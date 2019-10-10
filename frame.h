#ifndef FRAME_H
#define FRAME_H
#include <QDialog>
#include <qsystemtrayicon.h>
#define AROUND_STRETCH_FLAG

class QLabel;
class QPushButton;
namespace tools {
 class FramelessHelper;
}
class Frame : public QDialog
{
    Q_OBJECT
public:
    explicit Frame(QWidget *parent = 0);
    ~Frame();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void slot_max_clicked();
    void slot_systrayIcon_active(QSystemTrayIcon::ActivationReason);
private:
    void initUi();
    void initStyle();
    void initSysIcon();

    QSystemTrayIcon *m_systrayIcon;
    QWidget *m_title_widget;
    QLabel *m_icon_label;
    QLabel *m_title_label;
    QPushButton *m_max_button;
    QPushButton *m_min_button;
    QPushButton *m_close_button;
    QWidget *m_main_widget;
    tools::FramelessHelper * m_framelessHelper;
};

#endif // FRAME_H
