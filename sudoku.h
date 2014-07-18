#ifndef SUDOKU_H
#define SUDOKU_H
#include <vector>
#include <set>

typedef std::set<int> Node;
typedef std::vector< Node > Grid;
typedef std::vector< Node* > Group;

class Sudoku
{

private:
    Grid grid;
    int N,S,R,C,SR,SC,NSH,NSV;
    std::set<int> bad;
    std::set<int> given;
    int nsolutions;
    
private:
    Group Row (int);
    Group Column (int);
    Group SubGrid (int);
    
private:
    void GroupCheck (int,int);   
    void RandomCheck (bool count=false);
    bool generateGrid_private (int, Node*, std::vector<int>);
    void findBadNodes_private (int[]);
    bool Solve_private (bool count=false);
    
public:
    bool Solve ();
    int nSolutions ();
    void findBadNodes ();
    bool failed ();
    const Node& GetNode (int,int) const;
    void generateGrid (int);
    const std::set<int>& badNodes () const;
    
public:
    Sudoku(int,int,int,int,int[]);
    Sudoku(int,int,int,int);

private:
    Sudoku(int,int,int,int,Grid);

};

#endif // SUDOKU_H
