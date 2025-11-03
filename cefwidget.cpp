#include "cefwidget.h"

#include <QDatetime>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QScreen>
#include <QWheelEvent>
#include <QWindow>

#include "base/cef_callback.h"
#include "wrapper/cef_closure_task.h"

CefWidget::CefWidget(QWidget* parent)
    : QOpenGLWidget(parent),
    texture_(nullptr)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    timer_ = new QTimer(this);
    timer_->setInterval(40);
    timer_->setSingleShot(false);
    connect(timer_, &QTimer::timeout, this, &CefWidget::OnTimeout);

    debounce_timer_ = new QTimer(this);
    debounce_timer_->setSingleShot(true);
    debounce_timer_->setInterval(500);
    connect(debounce_timer_, &QTimer::timeout, this, &CefWidget::NotifyResizeToCEF);
}

CefWidget::~CefWidget()
{
    if (client_)
    {
        client_->CloseBrowser();
        client_ = nullptr;
    }
}

void CefWidget::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    mt_.lock();
    if (!frame_buffer_ || width != width_ || height != height_) {
        if (frame_buffer_) {
            delete[] frame_buffer_;
            frame_buffer_ = nullptr;
        }
        if (front_frame_buffer_) {
            delete[] front_frame_buffer_;
            front_frame_buffer_ = nullptr;
        }

        frame_buffer_ = new uchar[width * height * 4];
        memset(frame_buffer_, 0, width * height * 4);
        front_frame_buffer_ = new uchar[width * height * 4];
        memset(front_frame_buffer_, 0, width * height * 4);

        if (width != width_ || height != height_) {
            memcpy(front_frame_buffer_, buffer, width * height * 4);
            need_recreate_texture_ = true;
        }

        width_ = width;
        height_ = height;

        if (!timer_->isActive()) {
            QMetaObject::invokeMethod(this, [this]() {
                timer_->start();
                });
        }
    }
    memcpy(frame_buffer_, buffer, width * height * 4);
    mt_.unlock();
}

void CefWidget::showEvent(QShowEvent* event)
{
    CreateBrowser();
}


void CefWidget::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    const QRect& q_rect = this->geometry();
    rect.x = q_rect.x();
    rect.y = q_rect.y();
    rect.width = q_rect.width();
    rect.height = q_rect.height();
}

void CefWidget::mouseDoubleClickEvent(QMouseEvent* event)
{

}

void CefWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
        [](CefRefPtr<QtCefClient> client, CefMouseEvent mouse_event) {
            if (client && client->GetBrowser() && client->GetBrowser()->GetHost())
                client->GetBrowser()->GetHost()->SendMouseMoveEvent(mouse_event, false);
        },
        client_, mouse_event));
}

void CefWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    if (event->button() == CefBrowserHost::MouseButtonType::MBT_RIGHT)
    {
        mouse_event.x = event->globalPos().x();
        mouse_event.y = event->globalPos().y();
    }

    CefBrowserHost::MouseButtonType type = event->button() == Qt::LeftButton ? CefBrowserHost::MouseButtonType::MBT_LEFT : CefBrowserHost::MouseButtonType::MBT_RIGHT;
    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
        [](CefRefPtr<QtCefClient> client, CefMouseEvent mouse_event, CefBrowserHost::MouseButtonType type) {
            if (client && client->GetBrowser() && client->GetBrowser()->GetHost())
                client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, true, 1);
        },
        client_, mouse_event, type));
}

void CefWidget::mousePressEvent(QMouseEvent* event)
{
    QPointF point = event->pos();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    CefBrowserHost::MouseButtonType type = event->button() == Qt::LeftButton ? CefBrowserHost::MouseButtonType::MBT_LEFT : CefBrowserHost::MouseButtonType::MBT_RIGHT;
    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
        [](CefRefPtr<QtCefClient> client, CefMouseEvent mouse_event, CefBrowserHost::MouseButtonType type) {
            if (client && client->GetBrowser() && client->GetBrowser()->GetHost())
                client->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, false, 1);
        },
        client_, mouse_event, type));
}

void CefWidget::CreateBrowser()
{
    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 30;
    client_ = new QtCefClient(this);
    CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<QtCefClient> client, CefBrowserSettings browser_settings) {
        CefWindowInfo window_info;
        window_info.windowless_rendering_enabled = 1;
        CefBrowserHost::CreateBrowser(window_info, client, "https://www.baidu.com", browser_settings, nullptr, nullptr);
        },
        client_, browser_settings));
}

void MyFunc(int arg) {
    int leno = arg;
}

void CefWidget::keyPressEvent(QKeyEvent* event)
{
    CefKeyEvent key_event;
    key_event.type = KEYEVENT_RAWKEYDOWN;
    key_event.windows_key_code = event->nativeVirtualKey();
    key_event.native_key_code = event->nativeScanCode();

    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
        [](CefRefPtr<QtCefClient> client, CefKeyEvent key_event) {
            if (client && client->GetBrowser() && client->GetBrowser()->GetHost())
                client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
        }, client_, key_event));

    if (!event->text().isEmpty() && event->text()[0].isPrint()) {
        CefKeyEvent char_event = key_event;
        char_event.type = KEYEVENT_CHAR;
        char_event.character = event->text()[0].unicode();
        CefPostTask(CefThreadId::TID_UI, base::BindOnce(
            [](CefRefPtr<QtCefClient> client, CefKeyEvent key_event) {
                if (client && client->GetBrowser() && client->GetBrowser()->GetHost())
                    client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
            }, client_, char_event));
    }

    QWidget::keyPressEvent(event);
}

void CefWidget::keyReleaseEvent(QKeyEvent* event)
{
    CefKeyEvent key_event;
    key_event.type = KEYEVENT_KEYUP;
    key_event.windows_key_code = event->nativeVirtualKey();
    key_event.native_key_code = event->nativeScanCode();


    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
        [](CefRefPtr<QtCefClient> client, CefKeyEvent key_event) {
            if (client && client->GetBrowser() && client->GetBrowser()->GetHost())
                client->GetBrowser()->GetHost()->SendKeyEvent(key_event);
        }, client_, key_event));
}

void CefWidget::wheelEvent(QWheelEvent* event)
{
    CefMouseEvent mouse_event;
    QPointF pos = event->position();
    mouse_event.x = pos.x();
    mouse_event.y = pos.y();

    int delta_x = event->angleDelta().x();
    int delta_y = event->angleDelta().y();
    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
        [](CefRefPtr<QtCefClient> client, CefMouseEvent mouse_event, int delta_x, int delta_y) {
            if (client && client->GetBrowser())
                client->GetBrowser()->GetHost()->SendMouseWheelEvent(mouse_event, delta_x, delta_y);
        },
        client_, mouse_event, delta_x, delta_y));

}

bool CefWidget::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info)
{
    Qt::CursorShape shape;
    switch (type) {
    case CT_POINTER:
        shape = Qt::CursorShape::ArrowCursor;
        break;
    case CT_HAND:
        shape = Qt::CursorShape::PointingHandCursor;
        break;
    case CT_IBEAM:
        shape = Qt::CursorShape::IBeamCursor;
        break;
    default:
        break;
    }

    QMetaObject::invokeMethod(this, [this, shape]() {
        setCursor(shape);
    });

    return true;
}

void CefWidget::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    QWindow* window = windowHandle();
    if (window != nullptr) {
        QScreen* screen = window->screen();
        ratio_ = screen->devicePixelRatio();
        screen_info.device_scale_factor = ratio_;
    }
}

bool CefWidget::Close()
{
    if (!client_) {
        close();
        return true;
    }

    client_->CloseBrowser();
    return false;
}

void CefWidget::closeEvent(QCloseEvent* event) {
    int leno = 10;
}

//void CefWidget::resizeEvent(QResizeEvent* event)
//{
//    if (client_ && client_->GetBrowser())
//    {
//        client_->GetBrowser()->GetHost()->WasResized();
//    }
//}

void CefWidget::initializeGL() {
    initializeOpenGLFunctions();

    // 简单顶点 + 纹理坐标
    GLfloat vertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 0.0f,
    };

    vbo_.create();
    vbo_.bind();
    vbo_.allocate(vertices, sizeof(vertices));

    vao_.create();
    vao_.bind();

    shader_.addShaderFromSourceCode(QOpenGLShader::Vertex,
        R"(
        attribute vec2 position;
        attribute vec2 texCoord;
        varying vec2 vTexCoord;
        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
            vTexCoord = texCoord;
        })");

    shader_.addShaderFromSourceCode(QOpenGLShader::Fragment,
        R"(
        uniform sampler2D tex;
        varying vec2 vTexCoord;
        void main() {
            gl_FragColor = texture2D(tex, vTexCoord);
        })");

    shader_.link();

    shader_.bind();

    int posLoc = shader_.attributeLocation("position");
    int texLoc = shader_.attributeLocation("texCoord");

    shader_.enableAttributeArray(posLoc);
    shader_.enableAttributeArray(texLoc);

    shader_.setAttributeBuffer(posLoc, GL_FLOAT, 0, 2, 4 * sizeof(GLfloat));
    shader_.setAttributeBuffer(texLoc, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));

    vao_.release();
    vbo_.release();
    shader_.release();
}

void CefWidget::resizeGL(int w, int h) {
    timer_->stop();
    qInfo() << "w: " << w << " h: " << h;
    glViewport(0, 0, w, h);

    if (!debounce_timer_) {
        return;
    }

    debounce_timer_->start();
}

void CefWidget::NotifyResizeToCEF() {
    CefPostTask(CefThreadId::TID_UI, base::BindOnce([](CefRefPtr<QtCefClient> client) {
        if (client && client->GetBrowser())
        {
            client->GetBrowser()->GetHost()->WasResized();
        }
        }, client_));

}

void CefWidget::paintGL() {
    mt_.lock();
    UpdateFrame(front_frame_buffer_, width_, height_);
    mt_.unlock();


    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!texture_ || !texture_->isCreated()) return;

    shader_.bind();
    vao_.bind();

    glActiveTexture(GL_TEXTURE0);
    texture_->bind();
    shader_.setUniformValue("tex", 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    texture_->release();
    vao_.release();
    shader_.release();
}

void CefWidget::UpdateFrame(const uchar* buffer, int width, int height) {
    if (!buffer) {
        return;
    }
    if (!texture_ || need_recreate_texture_) {
        if (texture_) {
            delete texture_;
        }

        texture_ = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture_->setFormat(QOpenGLTexture::RGBA8_UNorm); // 内部存储格式
        texture_->setSize(width, height);
        texture_->allocateStorage();
        texture_->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        texture_->setWrapMode(QOpenGLTexture::ClampToEdge);

        need_recreate_texture_ = false;
    }

    texture_->bind();

    texture_->setData(QOpenGLTexture::QOpenGLTexture::BGRA, QOpenGLTexture::UInt8, buffer);

    texture_->release();
}

void CefWidget::OnTimeout() {
    mt_.lock();
    if (!frame_buffer_ || !front_frame_buffer_) {
        mt_.unlock();
        return;
    }
    memcpy(front_frame_buffer_, frame_buffer_, width_ * height_ * 4);

    //static int frame_count = 0;
    //QImage img(front_frame_buffer_, width_, height_, QImage::Format_ARGB32);
    //img.save(QString("testpng/testpng_%1.png").arg(frame_count), "PNG");
    //frame_count++;
    mt_.unlock();

    update();
}

void CefWidget::CanClose()
{
    if (client_) {
        client_ = nullptr;
    }
    QMetaObject::invokeMethod(this, [this]() {
        emit browserReadyToClose();
        });
}