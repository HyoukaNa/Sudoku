#include "mainwindow.h"
#include "sudoku.h"
#include <time.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QFrame *groupBox = new QFrame;
    QGridLayout *mainLayout = new QGridLayout;
    QGridLayout *layout[SubGridRows][SubGridRows];
    QFrame *subGroupBox[SubGridRows][SubGridRows];

    level = medium;

    srand(time(NULL));
    
    // An input validator so only digits 1-9 may be entered.

    QRegExp rx("[1-9]\\d{0,0}");
    QValidator *validator = new QRegExpValidator(rx, this);

    // Create QPalette here and set its Color and Color Role
    colors[0]=Qt::darkYellow; // user input
    colors[1]=Qt::blue; // game values
    colors[2]=Qt::red; // conflicts
    colors[3]=Qt::green; // computer solved

    for (int i = 0;i < 4; ++i)
    {
        palettes[i] = new QPalette();
        palettes[i]->setColor(QPalette::Text,colors[i]); // user input
    }
	
    // Define the font to be used

    QFont *font = new QFont();
    font->setFamily("Helvetica");
    font->setPixelSize(20);
    QApplication::setFont(*font);

    // Create the grid display with boxes to enter digits.

    for (int i = 0; i < SubGridRows; ++i) {
        for (int j = 0; j < SubGridRows; ++j) {
            layout[i][j] = new QGridLayout;
            subGroupBox[i][j] = new QFrame;
            for (int k = 0; k < SubGridRows; ++k) {
                for (int l = 0; l < SubGridRows; ++l) {
                    int x = k+i*SubGridRows, y = l+j*SubGridRows;
                    entries[x][y] = new QLineEdit();
                    entries[x][y]->setAlignment(Qt::AlignCenter);
                    entries[x][y]->setFixedSize(32,32);
                    entries[x][y]->setValidator(validator);
                    entries[x][y]->setPalette(*palettes[0]);
                    entries[x][y]->setFrame(0);
                    connect(entries[x][y], SIGNAL(textEdited(const QString&)), this, SLOT(resetEntryColor()));
                    layout[i][j]->addWidget(entries[x][y],k,l);
                }
            }

            layout[i][j]->setSpacing ( 0 );
            layout[i][j]->setContentsMargins ( 0, 0, 0, 0 );
            subGroupBox[i][j]->setLayout(layout[i][j]);
            subGroupBox[i][j]->setFrameShape (QFrame::Box);

            mainLayout->addWidget(subGroupBox[i][j],i,j);
        }
    }

    // The solve, create, clear and reset

    QPushButton *solveButton = new QPushButton(tr("Solve"));
    QPushButton *clearButton = new QPushButton(tr("Clear"));
    QPushButton *createButton = new QPushButton(tr("Create"));
    QPushButton *resetButton = new QPushButton(tr("Reset"));
    solveButton->setFlat(1);
    clearButton->setFlat(1);
    resetButton->setFlat(1);
    createButton->setFlat(1);
	
    connect(solveButton, SIGNAL(clicked()), this, SLOT(solve()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(createButton, SIGNAL(clicked()), this, SLOT(create()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(solveButton);
    buttonLayout->addWidget(clearButton);
    QFrame *buttonBox = new QFrame;
    buttonBox->setLayout(buttonLayout);

    buttonBox->setFixedSize(360,50);
    mainLayout->addWidget(buttonBox,3,0,1,3);
    mainLayout->setSpacing( 0 );
    mainLayout->setContentsMargins(0,0,0,0);
    groupBox->setLayout(mainLayout);
	
    this->setCentralWidget(groupBox);

    openAct = new QAction(tr("&Open"), this);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  
    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    
    easyAct = new QAction(tr("&Easy"), this);
    easyAct->setCheckable(true);
    connect(easyAct, SIGNAL(triggered()), this, SLOT(setEasy()));

    mediumAct = new QAction(tr("&Medium"), this);
    mediumAct->setCheckable(true);
    mediumAct->setChecked(1);
    connect(mediumAct, SIGNAL(triggered()), this, SLOT(setMedium()));

    hardAct = new QAction(tr("&Hard"), this);
    hardAct->setCheckable(true);
    connect(hardAct, SIGNAL(triggered()), this, SLOT(setHard()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);

    difficultyMenu = menuBar()->addMenu(tr("&Difficulty"));
    difficultyMenu->addAction(easyAct);
    difficultyMenu->addAction(mediumAct);
    difficultyMenu->addAction(hardAct);

#ifdef __APPLE__
    this->setFixedSize(360,420);
#else
    this->setFixedSize(360,440);
#endif
    
    this->setWindowTitle(tr("Sudoku Machine"));

    QPalette backgPalette = this->palette();
    backgPalette.setColor(QPalette::Window, Qt::gray);
    this->setPalette(backgPalette);

    this->setUnifiedTitleAndToolBarOnMac (true);

    return;
}

void MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save sudoku. Of mighty importance"), "",
                                                    tr("Sudokus (*.sud);;All Files (*)"));
    
    if (fileName.isEmpty())
        return;
    else 
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) 
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream out(&file);

        int N = SubGridRows*SubGridRows;
        QString state;
        for (int i = 0; i < N; ++i) 
        {
            for (int j = 0; j < N; ++j) 
            {
                QColor c = entries[i][j]->palette().color(QPalette::Text);
                for (int k = 0; k < 4; ++k)
                    if (colors[k]==c)
                    {
                        QString s; s.setNum(k);
                        state.append(s);
                    }
                if (entries[i][j]->text().length()>0)
                    state.append(entries[i][j]->text());
                else state.append(tr("0"));
            }
        }
        
        out << state;
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Retrieve forgotten sudoku"), "",
                                                    tr("Sudokus (*.sud);;All Files (*)"));
    
    if (fileName.isEmpty())
        return;
    else 
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) 
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream in(&file);
       
        int N = SubGridRows*SubGridRows;
        QString state;
        state = in.readLine();
        
        if (state.length()!=2*N*N)
        {
            QMessageBox::information(this, tr("Look behind you"),tr("Your file has been corrupted!"));
            return;
        }

        this->clear();
        
        for (int i = 0; i < state.length(); i++)
        {
            QString s(state[i]);
            int k = (i-i%2)/2;
            entries[(k-k%N)/N][k%N]->setPalette(*palettes[s.toInt()]);
            i++;
            s = QString(state[i]);
            if (s!=tr("0"))
                entries[(k-k%N)/N][k%N]->setText(s);
        }
    }
}

void MainWindow::setEasy()
{
    level = easy;
    mediumAct->setChecked(0);
    hardAct->setChecked(0);
}

void MainWindow::setMedium()
{
    level = medium;
    easyAct->setChecked(0);
    hardAct->setChecked(0);
}

void MainWindow::setHard()
{
    level = hard;
    easyAct->setChecked(0);
    mediumAct->setChecked(0);
}

void MainWindow::solve()
{
    int N = SubGridRows*SubGridRows;
    int grid[N*N];

    // Get input from the grid
    bool blue_nodes=false;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (entries[i][j]->palette().color(QPalette::Text)==colors[1])
            {
                grid[j+i*N] = -entries[i][j]->text().toInt();
                blue_nodes = true;
            }
            else 
            {
                grid[j+i*N] = entries[i][j]->text().toInt();               
            }
        }
    }

    // Create sudoku object and solve the shit out of it

    Sudoku sud(SubGridRows,SubGridRows,SubGridRows,SubGridRows,grid);
    sud.findBadNodes();

    if ( sud.Solve() ) // if solution exists
    {
        for (int i = 0; i < N; ++i)
        { // Enter solution on the grid
            for (int j = 0; j < N; ++j)
            {
                if (grid[j+i*N]==0)
                    entries[i][j]->setPalette(*palettes[3]);

                QString *str = new QString();
                str->setNum(*sud.GetNode(i,j).begin());
                entries[i][j]->setText(*str);
            }
        }
    } 
    else if (blue_nodes) // if it doesn't, highlight what went wrong if there
    {                    // was a sudoku board given
        int g[N*N];
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (grid[j+i*N]<0)
                {
                    grid[j+i*N] *= -1;
                    g[j+i*N] = grid[j+i*N];
                }
                else
                {
                    g[j+i*N] = 0;
                }
            }
        }
        Sudoku origSud(SubGridRows,SubGridRows,SubGridRows,SubGridRows,g);
        origSud.Solve();
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (*origSud.GetNode(i,j).begin()!=grid[i*N+j])
                {
                    entries[i][j]->setPalette(*palettes[2]);
                }
            }
        }
    }
    else // user given sudoku is bad
    {
        std::set<int> bad = sud.badNodes();
        std::set<int>::const_iterator it;
        for (it=bad.begin(); it!=bad.end(); ++it)
        {
            int n = *it;
            entries[(n-n%N)/N][n%N]->setPalette(*palettes[2]);
        }
    }
}

void MainWindow::clear() // clears the grid completely
{
    int N = SubGridRows*SubGridRows;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            entries[i][j]->clear();
            entries[i][j]->setPalette(*palettes[0]);
        }
    }
}

void MainWindow::resetEntryColor() // resets entry text color to default
{
    QPushButton* entry = (QPushButton*) QObject::sender();
    entry->setPalette(*palettes[0]);
}
	
void MainWindow::reset() // reset all entries which are not blue, i.e. not given numbers
{
    int N = SubGridRows*SubGridRows;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            QColor c = entries[i][j]->palette().color(QPalette::Text);
            if (c!=colors[1])
            {
                entries[i][j]->clear();
                entries[i][j]->setPalette(*palettes[0]);
            }
        }
    }
}

void MainWindow::create() // generates random sudoku puzzle with unique solution
{
    Sudoku sud(SubGridRows,SubGridRows,SubGridRows,SubGridRows);
    sud.generateGrid(level+rand()%5);

    int N = SubGridRows*SubGridRows;

    // Enter sudoku on the grid
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (sud.GetNode(i,j).size()==1)
            {
                QString *str = new QString();
                str->setNum(*sud.GetNode(i,j).begin());
                entries[i][j]->setText(*str);
                entries[i][j]->setPalette(*palettes[1]);
            }
            else
            {
                entries[i][j]->clear();
            }
        }
    }
}

MainWindow::~MainWindow()
{
}
