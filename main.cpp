#include <SFML/Graphics.hpp>
#include<vector>
#include<utility>
using namespace sf;
using namespace std;

const int board_size = 15;//格子数量
const int cell_size = 40;//格子大小
const int board_margin = 50;//边框距离
const int window_size =cell_size*board_size+board_margin*2;//屏幕大小

int board[board_size+1][board_size+1]={0};//1代表black，2代表white
bool isblack=true;
int winner=0;
bool GameOver=false;
vector<pair<int,int>> moveHistory;

//人工智障
bool isAI=true;
int scoreTable[6]={0,10,100,1000,10000,100000};//下棋得分

//判断是否五子成型
bool checkWin(int x,int y) {
    int dx[]={1,0,1,1};
    int dy[]={0,1,1,-1};
    int flag=board[x][y];
    for (int i=0;i<4;i++) {
        int count=1;
        for (int pd=1;pd<5;pd++){//pd==positive_dirction
            int nx=x+dx[i]*pd;
            int ny=y+dy[i]*pd;
            if (nx<0||ny<0||nx>board_size||ny>board_size) {
                break;
            }
            if (board[nx][ny]==flag) {
                count++;
            }
            else {
                break;
            }
        }
        for (int od=1;od<5;od++) {//od=opposive_dirction
            int nx=x-dx[i]*od;
            int ny=y-dy[i]*od;
            if (nx<0||ny<0||nx>board_size||ny>board_size) {
                break;
            }
            if (board[nx][ny]==flag) {
                count++;
            }
            else {
                break;
            }
        }
        if (count>=5) {
            return true;
        }
    }
    return false;
}
//分数评估
int evaluatePoint(int x,int y,int player) {
    int totalScore=0;
    int dx[]={1,0,1,1};
    int dy[]={0,1,1,-1};

    for (int dirction=0;dirction<4;dirction++){
        int count=1;
        int blocks=0;

        int nx=x+dx[dirction];
        int ny=y+dy[dirction];
        //向正方向移动
        while (nx>=0&&ny>=0&&nx<=board_size&&ny<=board_size&&board[nx][ny]==player) {
            count++;
            nx = nx+dx[dirction];
            ny = ny+dy[dirction];
        }
        if (nx<0||ny<0||nx>board_size||ny>board_size||board[nx][ny]!=player&&board[nx][ny]!=0) {
            blocks++;
        }

        nx=x-dx[dirction];
        ny=y-dy[dirction];
        //向反方向移动
        while (nx>=0&&ny>=0&&nx<=board_size&&ny<=board_size&&board[nx][ny]==player) {
            count++;
            nx = nx-dx[dirction];
            ny = ny-dy[dirction];
        }
        if (nx<0||ny<0||nx>board_size||ny>board_size||board[nx][ny]!=player&&board[nx][ny]!=0) {
            blocks++;
        }
        if (count>=5) {
            totalScore=totalScore+scoreTable[5];
        }
        else if (blocks==0) {
            totalScore=totalScore+scoreTable[count]*2;
        }
        else if (blocks==1) {
            totalScore=totalScore+scoreTable[count];
        }
    }
    return totalScore;
}
//人工智障
void aiMove(Text &text) {
    vector<pair<int,int>>emptyPoints;
    for (int i=0;i<=board_size;i++) {
        for (int j=0;j<=board_size;j++) {
            if (board[i][j]==0) {
                emptyPoints.push_back({i,j});
            }
        }
    }

    int bestScore=-1;
    int bestX=-1,bestY=-1;
    for (pair<int,int> point : emptyPoints) {
        int score=evaluatePoint(point.first,point.second,2);
        score=score+evaluatePoint(point.first,point.second,1);//如果这个点放黑棋子分也高，就形成了防守
        if (score>bestScore) {
            bestScore=score;
            bestX=point.first;
            bestY=point.second;
        }
    }
    board[bestX][bestY]=2;
    if (checkWin(bestX,bestY)) {
        GameOver=true;
        winner=2;
        text.setString("WHITE WINS");
    }
    isblack=!isblack;
}

int main() {
    RenderWindow window(VideoMode(window_size, window_size), "game");

    //设置文字显示
    Font font;
    font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    Text text;
    text.setFont(font);
    text.setCharacterSize(36);
    text.setColor(Color::Red);
    text.setPosition(250, window_size / 2 - 40);

    //预览棋子
    const float piece_radius = cell_size / 2 - 8;
    CircleShape preview(piece_radius);
    preview.setFillColor(Color(0, 0, 0, 150)); //半透明黑色


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            //下棋子
            if (GameOver==false&&event.type==Event::MouseButtonPressed && event.mouseButton.button==Mouse::Left){
                int mouseX=event.mouseButton.x;
                int mouseY=event.mouseButton.y;

                int x=(mouseX-board_margin+cell_size/2)/cell_size;
                int y=(mouseY-board_margin+cell_size/2)/cell_size;
                if (x>=0&&x<=board_size&&y>=0&&y<=board_size&&board[x][y]==0){
                    board[x][y]=isblack?1:2;//isblack==true就设置成1（black），isblack==flase就设置成2（white).
                    moveHistory.push_back({x,y});
                    if (checkWin(x,y)) {
                        GameOver=true;
                        winner=board[x][y];
                        text.setString(winner==1?"BLACK WINS":"WHITE WINS");
                    }
                    isblack=!isblack;
                }
            }


            //悔棋
            if (GameOver==false&&event.type==Event::KeyPressed&&event.key.code==Keyboard::U) {
                if (moveHistory.size()>=2) {
                    pair<int,int> pos1=moveHistory.back();
                    moveHistory.pop_back();
                    pair<int,int> pos2=moveHistory.back();
                    moveHistory.pop_back();
                    board[pos1.first][pos1.second]=0;
                    board[pos2.first][pos2.second]=0;

                }
            }
            //重开游戏
            if (event.type==Event::KeyPressed&&event.key.code==Keyboard::R) {
                GameOver=false;
                for (int i=0;i<=board_size;i++) {
                    for (int j=0;j<=board_size;j++) {
                        board[i][j]=0;
                    }
                }
                isblack=true;
                moveHistory.clear();
            }
        }

        //ai下棋
        if (isAI&&isblack==false&&GameOver==false) {
            aiMove(text);
        }
        window.clear(Color::White);

        // 画棋盘线
        for (int i = 0; i <= board_size; i++) {
            // 横线
            Vertex hLine[] = {
                Vertex(Vector2f(board_margin, board_margin + i * cell_size), Color::Black),
                Vertex(Vector2f(window_size - board_margin, board_margin + i * cell_size), Color::Black)
            };
            window.draw(hLine, 2, Lines);

            // 竖线
            Vertex vLine[] = {
                Vertex(Vector2f(board_margin + i * cell_size, board_margin), Color::Black),
                Vertex(Vector2f(board_margin + i * cell_size, window_size - board_margin), Color::Black)
            };
            window.draw(vLine, 2, Lines);
        }

        //画棋子
        for (int i=0;i<=board_size;i++) {
            for (int j=0;j<=board_size;j++) {
                if (board[i][j]!=0) {
                    CircleShape circle(cell_size/2-4);
                    circle.setFillColor(board[i][j]==1?Color::Black:Color::White);//判断是黑还是白
                    circle.setOutlineColor(Color::Black);
                    circle.setOutlineThickness(1);
                    circle.setPosition(board_margin+i*cell_size-circle.getRadius(),board_margin+j*cell_size-circle.getRadius());
                    window.draw(circle);
                }
            }
        }
        
        // 获取鼠标坐标 → 转换成棋盘坐标
        Vector2i mousePos = Mouse::getPosition(window);
        int x = (mousePos.x - board_margin + cell_size / 2) / cell_size;
        int y = (mousePos.y - board_margin + cell_size / 2) / cell_size;

        // 判断是否在合法范围内
        if (x >= 0 && x <= board_size && y >= 0 && y <= board_size) {
            if (board[x][y] == 0 && !GameOver) { // 该格为空且游戏未结束
                preview.setPosition(
                    board_margin + x * cell_size - piece_radius,
                    board_margin + y * cell_size - piece_radius
                );

                if (isblack)
                    preview.setFillColor(Color(0, 0, 0, 100)); // 半透明黑
                else {
                    preview.setFillColor(Color(255, 255, 255, 100));
                    preview.setOutlineColor(Color::Black);
                    preview.setOutlineThickness(1);
                }// 半透明，白
                window.draw(preview); // 画出来
            }
        }

        if (GameOver) {
            window.draw(text);
        }
        window.display();
    }
    return 0;
}