#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class QAction;
class QLineEdit;
class QMenu;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum { SubGridRows = 3 };

    QLineEdit *entries[SubGridRows*SubGridRows][SubGridRows*SubGridRows];
	
	QPalette *palettes[4];

    QMenu *fileMenu;
    QMenu *difficultyMenu;
    QAction *saveAct;
    QAction *openAct;
    QAction *easyAct;
    QAction *mediumAct;
    QAction *hardAct;

    int level;

    enum { easy=40, medium=45, hard=50 };

    QColor colors[4];

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void solve ();
    void clear ();
	void reset ();
	void resetEntryColor ();
    void create ();
    void save ();
    void open ();
    void setEasy ();
    void setMedium ();
    void setHard ();
};

#endif // MAINWINDOW_H
