#include <QPushButton>
#include <QTabBar>

class BrowserTabBar : public QTabBar {
  Q_OBJECT
 public:
  BrowserTabBar(QWidget* parent = nullptr);
  virtual ~BrowserTabBar() = default;

 protected:
  virtual void tabInserted(int index);

 protected slots:
  void OnNewTabButtonClicked();

 signals:
  void newTabClicked();

 protected:
  QPushButton* new_tab_button_ = nullptr;
};