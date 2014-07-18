#include "sudoku.h"
#include <stdlib.h>

// if entry in board is negative, then that node is 
// preferred good in the case of equal conflicts with another.
// if entry is positive, the node is regular.
// if entry is zero, then node is empty.
// dim1 = number of rows in a subgrid
// dim2 = number of columns in a subgrid
// dim3 = number of subgrids going vertically
// dim4 = number of subgrids going across
// i.e.
/* x x x | x x x
   x x x | x x x
   -------------
   x x x | x x x
   x x x | x x x
   -------------
   x x x | x x x
   x x x | x x x

   In this case dim1=2, dim2=3, dim3=3, dim4=2
   For a normal sudoku, dim1=dim2=dim3=dim4=3
*/
Sudoku::Sudoku(int dim1, int dim2, int dim3, int dim4, int board[])
{
    N = dim1*dim2*dim3*dim4;
    SR = dim1;
    SC = dim2;
    S = SR*SC;
    NSH = dim4;
    NSV = dim3;
    R = SR*NSV;
    C = SC*NSH;
    
    nsolutions=-1;
    
    int* numbers = new int[S];
    for (int i = 0; i < S; ++i) 
        numbers[i] = i+1;

    Node empty(numbers,numbers+S);

    for (int i = 0; i < N; ++i)
    {
        int x = board[i];
        if (x==0) 
        {
            grid.push_back(empty);
        } 
        else 
        {
            Node w;
            if (x<0)
            {
                w.insert(-x);
                given.insert(i);
            }
            else
            {
                w.insert(x);
            }
            grid.push_back(w);
        }
    }
}

Sudoku::Sudoku(int dim1, int dim2, int dim3, int dim4, Grid g)
{
    grid = g;
    
    N = dim1*dim2*dim3*dim4;
    SR = dim1;
    SC = dim2;
    S = SR*SC;
    NSH = dim4;
    NSV = dim3;
    R = SR*NSV;
    C = SC*NSH;
    
    nsolutions=-1;
}

Sudoku::Sudoku(int dim1, int dim2, int dim3, int dim4)
{
    N = dim1*dim2*dim3*dim4;
    SR = dim1;
    SC = dim2;
    S = SR*SC;
    NSH = dim4;
    NSV = dim3;
    R = SR*NSV;
    C = SC*NSH;
    
	nsolutions=-1;
    
	int* numbers = new int[S];
    for (int i = 0; i < S; ++i) 
    {
        numbers[i] = i+1;
    }

    Node empty(numbers,numbers+S);
    
	// Empty sudoku
    for (int i = 0; i < N; ++i)
    {
        grid.push_back(empty);
    }
}

void Sudoku::findBadNodes () // Finds immediate conflicts on the grid
{
    int board[N];
	for (int i = 0; i < N; ++i)
	{
        if (grid[i].size() == 1) 
        {
            board[i]=*grid[i].begin();
        }
        else 
        {
            board[i] = 0;
        }
	}
    findBadNodes_private (board);
}

void Sudoku::findBadNodes_private (int board[]) 
{
    // Used to count conflicts for each node
    int countConflicts[N];
    for (int i = 0; i < N; ++i)
        countConflicts[i] = 0;
    
    int newBoard[N];
    int r[C], c[R], sub[S];

    for (int i = 0; i < N; ++i)
        newBoard[i] = board[i];
    
    // Check if its a valid sudoku to start with
	
    for (int i = 0; i < C; ++i) // check columns for conflicts
    {
        for (int j = 0; j < R; ++j) 
            c[j] = board[j*C+i];
        
        for (int j = 0; j < R; ++j)
        {
            for (int k = j+1; k < R; ++k)
            {
                if (c[j]!=0 && c[k]!=0)
                    if (c[j]==c[k])
                    {
                        countConflicts[j*C+i]++;
                        countConflicts[k*C+i]++;
                    }
            }
        }
    }
        
    for (int i = 0; i < R; ++i) // check rows for conflicts
    {
        for (int j = 0; j < C; ++j) 
            r[j] = board[i*C+j];
        
        for (int j = 0; j < C; ++j)
        {
            for (int k = j+1; k < C; ++k)
            {
                if (r[j]!=0 && r[k]!=0)
                    if (r[j]==r[k])
                    {
                        countConflicts[i*C+j]++;
                        countConflicts[i*C+k]++;
                    }
            }
        }
    }
    
    int M = NSH*NSV;
    for (int i = 0; i < M; ++i) // check subgrids for conflicts
    {
        int a = ((i-i%NSH)/NSH)*SR;
        int b = (i%NSH)*SC;
        for (int j = 0; j < S; ++j)
        {// complicated expressions for row/col given which subgrid and where in that subgrid we are
            int row = a+((j-j%SC)/SC);
            int col = b+(j%SC);
            sub[j] = board[row*C+col];
        }
        
        for (int j = 0; j < S; ++j)
        {
            int rowj = a+((j-j%SC)/SC);
            int colj = b+(j%SC);
            for (int k = j+1; k < S; ++k)
            {
                if (sub[j]!=0 && sub[k]!=0)
                    if (sub[j]==sub[k])
                    {
                        int rowk = a+((k-k%SC)/SC);
                        int colk = b+(k%SC);
                        countConflicts[rowj*C+colj]++;
                        countConflicts[rowk*C+colk]++;
                    }
            }
        }
    }
	
    // For every node value, remove node with max conflicts greater than 0, preferably the ones which are positive in value
    bool bnf=false;
    for (int i = 0; i < S; ++i) 
    {
        int min=0, pos=-1;
        for (int j = 0; j < N; ++j)
        {
            if (board[j]!=i+1)
            {
                continue;
            }
                    
            if(countConflicts[j]>min)
            {
                min = countConflicts[j];
                pos=j;
            }
            else if(countConflicts[j]==min && min>0)
            {
                if(given.count(j)==0)
                    pos=j;
            }
        }
        
        if ( pos != -1 )
        {
            newBoard[pos]=0;
            bad.insert(pos);
            bnf = true;
        }
    }
    
    // if a bad node was found, continue until none are found
    if ( bnf )
    {
        findBadNodes_private (newBoard);
        nsolutions=0;
    }
}

const std::set<int>& Sudoku::badNodes () const
{
	return bad;
}

Group Sudoku::Row (int n)
{
    Group row;
    for (int i = 0; i < C; ++i)
    {
        row.push_back(&grid[n*C+i]);
    }
    return row;
}

Group Sudoku::Column (int n)
{
    Group col;
    for (int i = 0; i < R; ++i)
    {
        col.push_back(&grid[i*C+n]);
    }
    return col;
}

Group Sudoku::SubGrid (int n)
{
    Group sub;
    int a = ((n-n%NSH)/NSH)*SR;
    int b = (n%NSH)*SC;
    for (int i = 0; i < S; ++i) 
    {
        int row = a+((i-i%SC)/SC);
        int col = b+(i%SC);
        sub.push_back(&grid[row*C+col]);
    }
    return sub;
}

bool Sudoku::failed ()
{
    return nsolutions==0;
}

const Node& Sudoku::GetNode (int i,int j) const
{
    return grid[i*C+j];
}


///////////////// ** Sudoku solving procedures ** //////////////////

void Sudoku::GroupCheck (int n, int rcs)
{    
    Group group;
    int L;
    switch (rcs) // depending on wether you want to check
    {            // row column or subgrid
        case 0:
            group = Row(n);
            L = C;
            break;
        case 1:
            group = Column(n);
            L = R;
            break;
        case 2:
            group = SubGrid(n);
            L = S;
            break;
        default:
            return;
    }

    for (int i = 0; i < L; ++i) // if a node has only one possibility, delete
    {                           // corresponding possibility from every other node in the group
        if (group[i]->size()==1) 
        {
            for (int j = 0; j < L; ++j) 
            {
                if (i!=j) 
                {
                    group[j]->erase(*group[i]->begin());
                }
            }   
        }
    }
    
    // Check for numbers allowed only in one node within a group
    for (int i = 0; i < S; ++i)
    {
        int c = 0, k = 0;
        for (int j = 0; j < L; ++j) 
        {
            if (group[j]->count(i+1)) 
            {
                c++;
                k=j;
            }
        }
    
        if (c==1)
        {
            group[k]->clear();
            group[k]->insert(i+1);
        }
    }
    return;
}

void Sudoku::RandomCheck (bool count)
{
    int min=S+1, k=-1;
    for (int i = 0; i < N; ++i) // Pick a random node with least amount of possibilities
    {
        int m = grid[i].size();
            
        if (m==1) // if node has single possibility, continue
            continue;
            
        if (m < min || (rand()%5==0 && m==min))
        {   // pick random node with minimal possibilities
            k=i;
            min = m;
        }
    }

    if (k != -1) // if a multi possibility node exists, try a random number
    {
        Node node = grid[k];
        Node::iterator it;
        
        for (it=node.begin(); it!=node.end(); ++it) 
        {  // two ways of doing this: remove one possibility or pick one. here i pick
            grid[k].clear();
            grid[k].insert(*it);

            Sudoku trial(SR,SC,NSV,NSH,grid);
            
            if(trial.Solve_private(count))
            {
                nsolutions += trial.nsolutions;

                if (!count)
                {
                    grid = trial.grid;
                    return;
                }
            } 
        }
        grid[k]=node;
    }
    else 
    {
        nsolutions++;
    }
}
//////////// ** Solving routine ** //////////////////

bool Sudoku::Solve ()
{
    Grid original(grid);
    bool solvable = Solve_private(false);
    // If no solution, set all original nodes to bad
    for (int i = 0; i < N && !solvable; ++i)
    {
        if(original[i].size()==1)
        {
            bad.insert(i);
        }
    }
    return solvable;
}

int Sudoku::nSolutions()
{
    if (nsolutions==-1)
    {
        Grid original(grid);
        Solve_private(true);
        grid = original;
    }
    return nsolutions;
}

bool Sudoku::Solve_private (bool count)
{
    if (nsolutions==0) return false; // If there are no solutions, no point in trying to solve
    
    nsolutions = 0; // Initialize number of solutions to 0
    
    while (1)
    {
        Grid copy(grid);
        
        static int L[3]={R,C,S};
        for (int i = 0; i < 3; ++i) // Check each group
            for (int j = 0; j < L[i]; ++j)
                GroupCheck(j,i);

        if (grid == copy) // if the grid hasn't changed
        {            
            RandomCheck(count);
            return nsolutions>0;
        }
    }
}

/* Sudoku unique solution grid generator */
bool Sudoku::generateGrid_private (int level, Node* empty,std::vector<int> positions)
{
    if (level==0)
    {
        return true;
    }
        
    while (1)
    { 
        if ((int)positions.size()<level)
        {
            return false;
        }
        
        int j = rand()%(positions.size());
        
        int i = positions[j];
        positions.erase (positions.begin()+j); 
        
        Node w = grid[i];
        
        grid[i] = *empty;
        
        nsolutions = -1;
        nsolutions = nSolutions();
        
        if (nsolutions==1)
        {
            if (generateGrid_private(level-1,empty,positions))
            {
                return true;
            }
        }
        grid[i]=w;
    }
}

void Sudoku::generateGrid (int level)
{ // generate random solution and remove entries while keeping a unique solution
    Sudoku sud(SR,SC,NSV,NSH);
    sud.Solve();
    grid = sud.grid;

    int* numbers = new int[S];
    for (int i = 0; i < S; ++i)
        numbers[i] = i+1;
    
    Node empty(numbers,numbers+S);
    
    std::vector<int> positions;
    for (int i = 0; i < N; ++i)
        positions.push_back(i);
    
    generateGrid_private(level,&empty,positions);
}
