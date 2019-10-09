#ifndef FRAME_H
#define FRAME_H
#include <QDialog>
//#define AROUND_STRETCH_FLAG

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

private:
    void initUi();
    void initStyle();

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
