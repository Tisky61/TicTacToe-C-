#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <thread>
#include <chrono>
#include <limits.h>
sf::RenderWindow window(sf::VideoMode(270, 270), "Jogo da Velha!");

enum Possibilidades {vazio,P_X,P_O,empate};
typedef class TicTacToeBoard {
public:
    bool Turno = false;
    Possibilidades tab[3][3] {{vazio,vazio,vazio},
                              {vazio,vazio,vazio},
                              {vazio,vazio,vazio}};

    bool Play(int _x,int _y) {
        if(tab[_x][_y]) return false;
        tab[_x][_y] = Turno?P_O:P_X;
        Turno=!Turno; return true;
    }
    Possibilidades Check(void) {
        ///Vertical e horizontal
        for(int i = 0; i != 3;++i) {
            if(tab[i][0]==tab[i][1] && tab[i][1]==tab[i][2] && tab[i][0]) return tab[i][0];
            if(tab[0][i]==tab[1][i] && tab[1][i]==tab[2][i] && tab[0][i]) return tab[0][i];
        }
        ///Diagonal
        if(tab[0][0]==tab[1][1] && tab[1][1] == tab[2][2] && tab[1][1]) return tab[1][1];
        if(tab[0][2]==tab[1][1] && tab[1][1] == tab[2][0] && tab[1][1]) return tab[1][1];
        ///Detecta se ainda é possivel fazer algum movimento
        for(auto&x : tab) {
            for(auto&y : x) {
                if(y==vazio)return vazio;
            }
        }
        ///Caso negativo declara empate
        return empate;
    }
} TicTabToeBoard;

TicTabToeBoard game;

///Minimax
int __Escolha(TicTabToeBoard board,bool maximizing_player) {
    const long long result = board.Check();
    if(result) if(game.Turno) return (int []){-1,1,0}[result-1]; ///Caso a IA seja bola
               else           return (int []){1,-1,0}[result-1]; ///Caso a IA seja cruz
    if(maximizing_player) {
        int valor = -INT_MAX;
        for(int i = 0; i != 3;++i)for(int j = 0; j != 3;++j) {
            TicTabToeBoard newboard = board;
            if(newboard.Play(i,j))
                valor = std::max(valor,__Escolha(newboard,false));
        }
        return valor;
    }else {
        int valor = INT_MAX;
        for(int i = 0; i != 3;++i)for(int j = 0; j != 3;++j) {
            TicTabToeBoard newboard = board;
            if(newboard.Play(i,j))
                valor = std::min(valor,__Escolha(newboard,true));
        }
        return valor;
    }
}
bool Jogar(void) {
    long long jg = -INT_MAX;
    std::pair<int,int> dec;
    for(int i = 0;i != 3;++i) {
        for(int j = 0; j != 3;++j) {
            TicTabToeBoard newboard = game;
            if(newboard.Play(i,j)) {
                int k = __Escolha(newboard,false);
                if(k>jg) {
                    jg = k;
                    dec = std::pair<int,int>(i,j);
                }
            }
        }
    }
    return game.Play(dec.first,dec.second);
}
sf::Texture texturas[3];
int main()
{
    #define IA_PLAYS
    #define DRAW()window.clear();\
        for(int i = 0; i != 3;++i)\
            for(int j = 0; j != 3;++j) {\
                sf::RectangleShape rs(sf::Vector2f(90,90));\
                rs.setPosition(sf::Vector2f(i*90,j*90));\
                rs.setTexture(&texturas[game.tab[i][j]]);\
                window.draw(rs);\
            }\
        window.display()
    texturas[0].loadFromFile("sprites/vazio.png");
    texturas[1].loadFromFile("sprites/risco.png");
    texturas[2].loadFromFile("sprites/bola.png" );

    while (window.isOpen())
    {
        static bool focused = true;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::LostFocus  ) focused = false;
            if (event.type == sf::Event::GainedFocus) focused = true;
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && focused) {
                sf::Vector2i pos = sf::Mouse::getPosition(window);
                if(
                   #ifndef IA_PLAYS
                        game.Play(floor((float)pos.x/(float)90),floor((float)pos.y/(float)90))
                   #else
                        Jogar()
                   #endif
                   ) {
                    #define PRINT_TABULEIRO()\
                    for(int i = 0;i != 3;++i){\
                        for(int j = 0;j!= 3;++j)\
                            printf("%d ",game.tab[j][i]);\
                        printf("\n");\
                    }
                    bool turnos = false;
                    rejulgar:
                    PRINT_TABULEIRO()
                    const int result = game.Check();
                    if(result) {
                       printf("Fim de jogo!!!Resultado: %s\n",((const char* []){"Vitoria do Jogador cruz!",
                                                                                "Vitoria do Jogador bola!",
                                                                                "Empate!"})[result-1]);
                       DRAW();
                       memset(&game,0,sizeof(game));
                       std::this_thread::sleep_for(std::chrono::seconds{3});
                       turnos = true;
                    }
                    if(!turnos) {
                    Jogar();turnos = true;goto rejulgar;}
                }
            }
        }

        DRAW();
    }
    return 0;
}
