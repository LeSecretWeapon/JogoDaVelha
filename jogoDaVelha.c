#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <locale.h>

//Variáveis Globais
char Gtabuleiro [5][5] = {{' ', '|', ' ', '|', ' '},
                         {'-', '+', '-', '+', '-'},
                         {' ', '|', ' ', '|', ' '},
                         {'-', '+', '-', '+', '-'},
                         {' ', '|', ' ', '|', ' '}};
char Gposicoes1[5][5], Gposicoes2[5][5], GmenuResposta;

//Estruturas
struct Gestatisticas
{
    int partidasJogadasTotal;
    int J1partidasJogadas;
    int J2partidasJogadas;
    int PCpartidasJogadas;
    int J1partidasGanhas;
    int J2partidasGanhas;
    int PCpartidasGanhas;
    int J1partidasPerdidas;
    int J2partidasPerdidas;
    int PCpartidasPerdidas;
    int J1partidasVelhas;
    int J2partidasVelhas;
    int PCpartidasVelhas;
    float J1porcentVitorias;
    float J2porcentVitorias;
    float PCporcentVitorias;
    char ultimoTabuleiro[5][5];
    char ultimoVitorioso[12];
};

struct Gestatisticas stats = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Declaração das Funções
void centralizarTexto(char texto[], int tamanho);
void linhaH(void);
int menu(void);
void tutorial(void);
void printarTabuleiro(char tabuleiro[5][5]);
int escolherPecaRand();
int escolherPecaIA();
void colocarPeca(int posicao, int usuario);
int checarVitoria(int usuario, char posicoes[5][5]);
void resetarJogo(int rodada, int usuario, int vitoria, char simbolo);
void salvarStats(int vitoria, int tipo);
void menuStats(void);
void pegarStats(FILE* arquivo);
void JogoVelhaPvp(void);
void JogoVelhaPve(void);

//Função Main
int main()
{
    system("cls");
    setlocale(LC_ALL, "portuguese");
    FILE *fStats;
    pegarStats(fStats);

    //Copia o array do tabuleiro vazio pros tabuleiros individuais
    memcpy(Gposicoes1, Gtabuleiro, sizeof(Gtabuleiro));
    memcpy(Gposicoes2, Gtabuleiro, sizeof(Gtabuleiro));

    GmenuResposta = menu();

    switch(GmenuResposta)
    {
    case '1':
        system("cls");
        JogoVelhaPvp();
        break;
    case '2':
        system("cls");
        JogoVelhaPve();
        break;
    case '3':
        system("cls");
        menuStats();
        break;
    case '4':
        system("cls");
        tutorial();
        break;
    case '5':
        exit(0);
        break;
    }
}

//Funções
void centralizarTexto(char texto[], int tamanho)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    int largura = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int pad = (tamanho >= largura) ? 0 : (largura - tamanho) / 2;

    printf("%*.*s%s\n", pad, pad, "", texto);
}

int tamanhoPad(int tamanho)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    int largura = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    if(tamanho == 120){
        return largura;
    }else{
    largura = (largura-tamanho)/2;
    return largura;
    }
}

void linhaH(void)
{
    int i;
    int largura = tamanhoPad(120);

    for(i = 0; i < largura; i++){
        printf("=");
    }
    printf("\n\n\b");
}

int menu(void)
{
    int resposta;

    centralizarTexto("JOGO DA VELHA", 13);
    printf("\n");
    centralizarTexto("Desenvolvido por: Vinícius Godoy", 32);
    printf("\n");
    centralizarTexto("Jogar com um Amigo(Pressionar 1)", 32);
    centralizarTexto("Jogar com o Computador(Pressionar 2)", 36);
    centralizarTexto("Ver estatísticas(Pressionar 3)", 30);
    centralizarTexto("Como Jogar(Pressionar 4)", 25);
    centralizarTexto("Sair(Pressionar 5)", 18);
    printf("\n%*.s", tamanhoPad(0), "");
    do{
    resposta = getch();
    }while(resposta != '1' && resposta != '2' && resposta != '3' && resposta !='4' && resposta != '5' && resposta != '`');

    return resposta;
}

void tutorial(void)
{
    centralizarTexto("COMO JOGAR\n", 10);
    printf("Digite o número da posição onde deseja colocar seu símbolo (X ou O), as posições são enumeradas conforme mostrado abaixo\n");
    //Muda o tabuleiro pra mostrar as posições enumeradas
    Gtabuleiro[0][0] = '1';Gtabuleiro[0][2] = '2';Gtabuleiro[0][4] = '3';
    Gtabuleiro[2][0] = '4';Gtabuleiro[2][2] = '5';Gtabuleiro[2][4] = '6';
    Gtabuleiro[4][0] = '7';Gtabuleiro[4][2] = '8';Gtabuleiro[4][4] = '9';
    printarTabuleiro(Gtabuleiro);
    printf("\n");
    centralizarTexto("OBJETIVO\n", 8);
    printf("Alinhe três dos mesmos símbolos (X ou O) seguidos, seja diagonal, vertical ou horizontalmente. Como indicado abaixo\n");
    //Muda o tabuleiro pra mostrar uma vitória
    Gtabuleiro[0][0] = 'X';Gtabuleiro[0][2] = ' ';Gtabuleiro[0][4] = ' ';
    Gtabuleiro[2][0] = ' ';Gtabuleiro[2][2] = 'X';Gtabuleiro[2][4] = ' ';
    Gtabuleiro[4][0] = ' ';Gtabuleiro[4][2] = ' ';Gtabuleiro[4][4] = 'X';
    printarTabuleiro(Gtabuleiro);
    printf("\n");
    printf("Caso nenhum dos jogadores consiga completar uma sequência de três símbolos o jogo dá 'velha', em outras palavras, empata\n");
    //Reseta o tabuleiro pro padrão
    Gtabuleiro[0][0] = ' ';Gtabuleiro[2][2] = ' ';Gtabuleiro[4][4] = ' ';
    centralizarTexto("BOM JOGO !\n\n", 10);
    centralizarTexto("PARA VOLTAR AO MENU PRESSIONE B\n", 32);
    printf("%*.s", tamanhoPad(0), "");
    do{
    GmenuResposta = getch();
    if(GmenuResposta == 'B' || GmenuResposta == 'b'){main();}
    }while(GmenuResposta != 'B' || GmenuResposta != 'b');
}

void printarTabuleiro(char tabuleiro[5][5])
{
    int i, j;

    for(i = 0; i < 5; i++){
        printf("%*.s", tamanhoPad(5), "");
        for(j = 0; j < 5; j++){
            printf("%c", tabuleiro[i][j]);
        }
        printf("\n");
    }
}

int escolherPecaRand()
{
    static int i = 0;
    int numpc;
    time_t t;

    ++i;
    if(i == 1){
        srand((unsigned) time(&t));
    }
    numpc = (rand()%9)+1;

    switch(numpc)
    {
    case 1:
        if(Gtabuleiro[0][0] == 'X' || Gtabuleiro[0][0] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 2:
        if(Gtabuleiro[0][2] == 'X' || Gtabuleiro[0][2] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 3:
        if(Gtabuleiro[0][4] == 'X' || Gtabuleiro[0][4] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 4:
        if(Gtabuleiro[2][0] == 'X' || Gtabuleiro[2][0] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 5:
        if(Gtabuleiro[2][2] == 'X' || Gtabuleiro[2][2] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 6:
        if(Gtabuleiro[2][4] == 'X' || Gtabuleiro[2][4] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 7:
        if(Gtabuleiro[4][0] == 'X' || Gtabuleiro[4][0] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 8:
        if(Gtabuleiro[4][2] == 'X' || Gtabuleiro[4][2] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    case 9:
        if(Gtabuleiro[4][4] == 'X' || Gtabuleiro[4][4] == 'O'){
            escolherPecaRand();
        }else{
            return numpc;
        }
        break;
    }
}

int escolherPecaIA()
{

}

void colocarPeca(int posicao, int usuario)
{
    char simb = ' ';

    if(usuario == 1){
        simb = 'X';
    }else if(usuario == 2 || usuario == 3){
        simb = 'O';
    }

    switch(posicao)
    {
    case 1:
        if(Gtabuleiro[0][0] == 'X' || Gtabuleiro[0][0] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[0][0] = simb;
            if(usuario == 1){
                Gposicoes1[0][0] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[0][0] = '1';
            }
        }
        break;
    case 2:
        if(Gtabuleiro[0][2] == 'X' || Gtabuleiro[0][2] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[0][2] = simb;
            if(usuario == 1){
                Gposicoes1[0][2] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[0][2] = '1';
            }
        }
        break;
    case 3:
        if(Gtabuleiro[0][4] == 'X' || Gtabuleiro[0][4] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[0][4] = simb;
            if(usuario == 1){
                Gposicoes1[0][4] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[0][4] = '1';
            }
        }
        break;
    case 4:
        if(Gtabuleiro[2][0] == 'X' || Gtabuleiro[2][0] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[2][0] = simb;
            if(usuario == 1){
                Gposicoes1[2][0] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[2][0] = '1';
            }
        }
        break;
    case 5:
        if(Gtabuleiro[2][2] == 'X' || Gtabuleiro[2][2] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[2][2] = simb;
            if(usuario == 1){
                Gposicoes1[2][2] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[2][2] = '1';
            }
        }
        break;
    case 6:
        if(Gtabuleiro[2][4] == 'X' || Gtabuleiro[2][4] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[2][4] = simb;
            if(usuario == 1){
                Gposicoes1[2][4] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[2][4] = '1';
            }
        }
        break;
    case 7:
        if(Gtabuleiro[4][0] == 'X' || Gtabuleiro[4][0] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[4][0] = simb;
            if(usuario == 1){
                Gposicoes1[4][0] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[4][0] = '1';
            }
        }
        break;
    case 8:
        if(Gtabuleiro[4][2] == 'X' || Gtabuleiro[4][2] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[4][2] = simb;
            if(usuario == 1){
                Gposicoes1[4][2] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[4][2] = '1';
            }
        }
        break;
    case 9:
        if(Gtabuleiro[4][4] == 'X' || Gtabuleiro[4][4] == 'O'){
            printf("\a[ERRO]Você digitou uma posição inválida ! Digite uma posição vazia: ");
            scanf("%i", &posicao);
            colocarPeca(posicao, usuario);
        }else{
            Gtabuleiro[4][4] = simb;
            if(usuario == 1){
                Gposicoes1[4][4] = '1';
            }else if(usuario == 2 || usuario == 3){
                Gposicoes2[4][4] = '1';
            }
        }
        break;
    default:
        printf("\a[ERRO]Você colocou um número de posição inexistente! Digite uma posição existente: ");
        scanf("%i", &posicao);
        colocarPeca(posicao, usuario);
    }
}

int checarVitoria(int usuario, char posicoes[5][5])
{
    int contvit = 0, contvit2 = 0, vit, i, j;

    if(usuario == 1){
        vit = 1;
    }else if(usuario == 2){
        vit = 2;
    }else if(usuario == 3){
        vit = 3;}

    //Checa as vitórias horizontais
    for(i = 0; i <= 4; i += 2){
        for(j = 0; j <= 4; j += 2){
            if(posicoes[i][j]== '1'){
                contvit++;
            }
        }
        if(contvit == 3){
            return vit;
        }else{
            contvit = 0;
        }
    }

    //Checa as vitórias verticais
    for(i = 0; i <= 4; i += 2){
        for(j = 0; j <= 4; j += 2){
            if(posicoes[j][i]== '1'){contvit++;}
        }
        if(contvit == 3){
            return vit;
        }else{contvit = 0;}
    }

    //Checa as vitórias diagonais
    for(i = 0; i <= 4; i += 2){
        if(i == 0){
            if(posicoes[i][0]== '1'){contvit++;}
            if(posicoes[i][4]== '1'){contvit2++;}
        }
        if(i == 2){
            if(posicoes[i][2]== '1'){contvit++; contvit2++;}
        }
        if(i == 4){
            if(posicoes[i][4]== '1'){contvit++;}
            if(posicoes[i][0]== '1'){contvit2++;}
        }
    }
    if(contvit == 3 || contvit2 == 3){return vit;}

    return 0;
}

void resetarJogo(int rodada, int usuario, int vitoria, char simbolo)
{
    int i, j;

    //Laço for que reseta o tabuleiro
    for(i = 0; i <= 5; i++){
        if(i%2 != 0){
        for(j = 0; j <= 5; j++){
            if(j%2 != 0){
                Gtabuleiro[i][j] = '+';
            }else{
                Gtabuleiro[i][j] = '-';
            }
        }
        }else{
        for(j = 0; j <= 5; j++){
            if(j%2 != 0){
                Gtabuleiro[i][j] = '|';
            }else{
                Gtabuleiro[i][j] = ' ';
            }
        }
        }
    }
    //Funções que copiam o tabuleiro principal pros individuais
    memcpy(Gposicoes1, Gtabuleiro, sizeof(Gtabuleiro));
    memcpy(Gposicoes2, Gtabuleiro, sizeof(Gtabuleiro));

    rodada = 1;
    usuario = 1;
    vitoria = 0;
    simbolo = 'X';
}

void salvarStats(int vitoria, int tipo)
{
    int i, j;
    FILE *fStats;

    stats.partidasJogadasTotal += 1;
    memcpy(stats.ultimoTabuleiro, Gtabuleiro, sizeof(Gtabuleiro));

    switch(tipo)
    {
    case 1:
        stats.J1partidasJogadas += 1;
        stats.J2partidasJogadas += 1;
        if(vitoria == 1){
            stats.J1partidasGanhas += 1;
            stats.J2partidasPerdidas += 1;
            stats.J1porcentVitorias = (stats.J1partidasGanhas/stats.J1partidasJogadas)*100;
            stats.J2porcentVitorias = (stats.J2partidasGanhas/stats.J2partidasJogadas)*100;
            strcpy(stats.ultimoVitorioso, "Jogador 1");
        }else if(vitoria == 2){
            stats.J2partidasGanhas += 1;
            stats.J1partidasPerdidas += 1;
            stats.J2porcentVitorias = (stats.J2partidasGanhas/stats.J2partidasJogadas)*100;
            stats.J1porcentVitorias = (stats.J1partidasGanhas/stats.J1partidasJogadas)*100;
            strcpy(stats.ultimoVitorioso, "Jogador 2");
        }else if(vitoria = 0){
            stats.J1partidasVelhas += 1;
            stats.J2partidasVelhas += 1;
            stats.J1porcentVitorias = (stats.J1partidasGanhas/stats.J1partidasJogadas)*100;
            stats.J2porcentVitorias = (stats.J2partidasGanhas/stats.J2partidasJogadas)*100;
        }
    break;
    case 2:
        stats.J1partidasJogadas += 1;
        stats.PCpartidasJogadas += 1;
        if(vitoria == 1){
            stats.J1partidasGanhas += 1;
            stats.PCpartidasPerdidas += 1;
            stats.J1porcentVitorias = (stats.J1partidasGanhas/stats.J1partidasJogadas)*100;
            stats.PCporcentVitorias = (stats.PCpartidasGanhas/stats.PCpartidasJogadas)*100;
            strcpy(stats.ultimoVitorioso, "Jogador 1");
        }else if(vitoria == 3){
            stats.PCpartidasGanhas += 1;
            stats.J1partidasPerdidas += 1;
            stats.PCporcentVitorias = (stats.PCpartidasGanhas/stats.PCpartidasJogadas)*100;
            stats.J1porcentVitorias = (stats.J1partidasGanhas/stats.J1partidasJogadas)*100;
            strcpy(stats.ultimoVitorioso, "Computador");
        }else if(vitoria = 0){
            stats.J1partidasVelhas += 1;
            stats.PCpartidasVelhas += 1;
            stats.J1porcentVitorias = (stats.J1partidasGanhas/stats.J1partidasJogadas)*100;
            stats.PCporcentVitorias = (stats.PCpartidasGanhas/stats.PCpartidasJogadas)*100;
        }
    break;
    }

    fStats = fopen("estatistícas.txt", "w");

    fprintf(fStats, "Total de partidas jogadas: %d\n", stats.partidasJogadasTotal);
    fprintf(fStats, "Partidas jogadas Jogador 1: %d\n", stats.J1partidasJogadas);
    fprintf(fStats, "Partidas jogadas Jogador 2: %d\n", stats.J2partidasJogadas);
    fprintf(fStats, "Partidas jogadas PC: %d\n", stats.PCpartidasJogadas);
    fprintf(fStats, "Partidas ganhas Jogador 1: %d\n", stats.J1partidasGanhas);
    fprintf(fStats, "Partidas ganhas Jogador 2: %d\n", stats.J2partidasGanhas);
    fprintf(fStats, "Partidas ganhas PC: %d\n", stats.PCpartidasGanhas);
    fprintf(fStats, "Partidas perdidas Jogador 1: %d\n", stats.J1partidasPerdidas);
    fprintf(fStats, "Partidas perdidas Jogador 2: %d\n", stats.J2partidasPerdidas);
    fprintf(fStats, "Partidas perdidas PC: %d\n", stats.PCpartidasPerdidas);
    fprintf(fStats, "Partidas empatadas Jogador 1: %d\n", stats.J1partidasVelhas);
    fprintf(fStats, "Partidas empatadas Jogador 2: %d\n", stats.J2partidasVelhas);
    fprintf(fStats, "Partidas empatadas PC: %d\n", stats.PCpartidasVelhas);
    fprintf(fStats, "Porcentagem vitórias Jogador 1: %f\n", stats.J1porcentVitorias);
    fprintf(fStats, "Porcentagem vitórias Jogador 2: %f\n", stats.J2porcentVitorias);
    fprintf(fStats, "Porcentagem vitórias PC: %f\n", stats.PCporcentVitorias);
    fprintf(fStats,"Último Tabuleiro vitorioso:\n");
    for(i = 0; i < 5; i++){
        for(j = 0; j < 5; j++){
            fprintf(fStats, "%c", stats.ultimoTabuleiro[i][j]);
        }
        fprintf(fStats, "\n");
    }
    fprintf(fStats, "Último vitorioso: %s\n", stats.ultimoVitorioso);

    fclose(fStats);
}

void menuStats(void)
{
    FILE *fStats;

    pegarStats(fStats);
    centralizarTexto("ESTATÍSTICAS", 12);
    printf("\n");
    printf("%*.sTOTAL DE PARTIDAS JOGADAS: %-2d\n\n", tamanhoPad(29), "", stats.partidasJogadasTotal);
    centralizarTexto("ULTÍMO JOGO", 11);
    printarTabuleiro(stats.ultimoTabuleiro);
    printf("%*.sCampeão: %s\n", tamanhoPad(18),"", stats.ultimoVitorioso);
    linhaH();
    centralizarTexto("JOGADOR 1", 9);
    printf("%*.sPartidas Jogadas: %-3d\n", tamanhoPad(21),"", stats.J1partidasJogadas);
    printf("%*.sPartidas Ganhas: %-3d\n", tamanhoPad(20),"", stats.J1partidasGanhas);
    printf("%*.sPartidas Perdidas: %-3d\n", tamanhoPad(22),"", stats.J1partidasPerdidas);
    printf("%*.sPartidas em que deu Velha: %-3d\n", tamanhoPad(30),"", stats.J1partidasVelhas);
    printf("%*.sPorcentagem de Vitórias: %-4.2f%%\n", tamanhoPad(30),"", stats.J1porcentVitorias);
    linhaH();
    centralizarTexto("JOGADOR 2", 9);
    printf("%*.sPartidas Jogadas: %-3d\n", tamanhoPad(21),"", stats.J2partidasJogadas);
    printf("%*.sPartidas Ganhas: %-3d\n", tamanhoPad(20),"", stats.J2partidasGanhas);
    printf("%*.sPartidas Perdidas: %-3d\n", tamanhoPad(22),"", stats.J2partidasPerdidas);
    printf("%*.sPartidas em que deu Velha: %-3d\n", tamanhoPad(30),"", stats.J2partidasVelhas);
    printf("%*.sPorcentagem de Vitórias: %-4.2f%%\n", tamanhoPad(30),"", stats.J2porcentVitorias);
    linhaH();
    centralizarTexto("COMPUTADOR", 10);
    printf("%*.sPartidas Jogadas: %-3d\n", tamanhoPad(21),"", stats.PCpartidasJogadas);
    printf("%*.sPartidas Ganhas: %-3d\n", tamanhoPad(20),"", stats.PCpartidasGanhas);
    printf("%*.sPartidas Perdidas: %-3d\n", tamanhoPad(22),"", stats.PCpartidasPerdidas);
    printf("%*.sPartidas em que deu Velha: %-3d\n", tamanhoPad(30),"", stats.PCpartidasVelhas);
    printf("%*.sPorcentagem de Vitórias: %-4.2f%%\n\n", tamanhoPad(30),"", stats.PCporcentVitorias);
    printf("%*.sPRESSIONE B PARA VOLTAR AO MENU", tamanhoPad(32), "");
    printf("\n%*.s", tamanhoPad(0), "");
    do{
    GmenuResposta = getch();
    if(GmenuResposta == 'b' || GmenuResposta == 'B'){
        system("cls");
        main();
    }
    }while(GmenuResposta != 'b' && GmenuResposta != 'B');
}

void pegarStats(FILE* arquivo)
{
    int i, j;

    arquivo = fopen("estatistícas.txt", "r");
    fseek(arquivo, 27, SEEK_SET);
    fscanf(arquivo,"%d", &stats.partidasJogadasTotal);
    fseek(arquivo, 29, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J1partidasJogadas);
    fseek(arquivo, 29, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J2partidasJogadas);
    fseek(arquivo, 22, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.PCpartidasJogadas);
    fseek(arquivo, 28, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J1partidasGanhas);
    fseek(arquivo, 28, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J2partidasGanhas);
    fseek(arquivo, 21, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.PCpartidasGanhas);
    fseek(arquivo, 30, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J1partidasPerdidas);
    fseek(arquivo, 30, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J2partidasPerdidas);
    fseek(arquivo, 23, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.PCpartidasPerdidas);
    fseek(arquivo, 31, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J1partidasVelhas);
    fseek(arquivo, 31, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.J2partidasVelhas);
    fseek(arquivo, 24, SEEK_CUR);
    fscanf(arquivo,"%d\n", &stats.PCpartidasVelhas);
    fseek(arquivo, 33, SEEK_CUR);
    fscanf(arquivo,"%f\n", &stats.J1porcentVitorias);
    fseek(arquivo, 33, SEEK_CUR);
    fscanf(arquivo,"%f\n", &stats.J2porcentVitorias);
    fseek(arquivo, 26, SEEK_CUR);
    fscanf(arquivo,"%f\n", &stats.PCporcentVitorias);
    fseek(arquivo, 29, SEEK_CUR);
    for(i = 0; i < 5; i++){
        for(j = 0; j < 5; j++){
            fscanf(arquivo,"%c", &stats.ultimoTabuleiro[i][j]);
        }
        //fseek(arquivo, 1, SEEK_CUR);
    }
    fseek(arquivo, 19, SEEK_CUR);
    fscanf(arquivo,"%%[^\n]s\n", &stats.ultimoVitorioso);

    fclose(arquivo);
}

void JogoVelhaPvp(void)
{
    do{
    int rodada = 1, usuario = 1, vitoria = 0, pos;
    char simbolo = 'X';

    while(rodada <= 9 && vitoria == 0){
        printf("%*.s%iº RODADA - JOGADOR %i\n", tamanhoPad(21), "", rodada, usuario);
        printarTabuleiro(Gtabuleiro);
        printf("%*.sDigite o número do espaço onde deseja colocar o %c: ", tamanhoPad(51), "", simbolo);
        scanf("%i", &pos);
        colocarPeca(pos, usuario);
        if(usuario == 1){vitoria = checarVitoria(usuario, Gposicoes1);}
        else if(usuario == 2){vitoria = checarVitoria(usuario, Gposicoes2);}
        rodada++;
        if(usuario == 1){
            usuario += 1;
            simbolo = 'O';
        }else{
            usuario -= 1;
            simbolo = 'X';
        }
        linhaH();
        }

        printarTabuleiro(Gtabuleiro);
        printf("\n");
        if(vitoria == 1){
            salvarStats(vitoria, 1);
            printf("%*.s\aParabéns Jogador 1 você ganhou !!!\n\n", tamanhoPad(34), "");
        }else if(vitoria == 2){
            salvarStats(vitoria, 1);
            printf("%*.s\aParabéns Jogador 2 você ganhou !!!\n\n", tamanhoPad(34), "");
        }else{
            salvarStats(0, 1);
            printf("%*.s\aDeu velha ! Ninguém ganhou :(\n\n", tamanhoPad(29), "");
        }
        printf("%*.sPRESSIONE R PARA JOGAR NOVAMENTE | PRESSIONE B PARA VOLTAR AO MENU", tamanhoPad(67), "");
        printf("\n%*.s", tamanhoPad(0), "");
        do{
        GmenuResposta = getch();
        if(GmenuResposta == 'b' || GmenuResposta == 'B'){
            system("cls");
            resetarJogo(rodada, usuario, vitoria, simbolo);
            main();
        }
        }while(GmenuResposta != 'r' && GmenuResposta != 'R' && GmenuResposta != 'b' && GmenuResposta != 'B');
        system("cls");
        resetarJogo(rodada, usuario, vitoria, simbolo);
        }while(GmenuResposta == 'r' || GmenuResposta == 'R');
}

void JogoVelhaPve(void)
{
    centralizarTexto("ESCOLHA A DIFICULDADE\n", 21);
    centralizarTexto("Aleatório (Pressione 1)\n", 23);
    centralizarTexto("IA Básica (Pressione 2)\n", 23);
    centralizarTexto("\n\nPRESSIONE B PARA VOLTAR", 23);
    printf("%*.s", tamanhoPad(0),"");
    do{
    GmenuResposta = getch();
    if(GmenuResposta == 'B' || GmenuResposta == 'b'){main();}
    }while(GmenuResposta != '1' && GmenuResposta != '2' && GmenuResposta != 'B' && GmenuResposta != 'b');

    switch(GmenuResposta)
    {
    case '1':
        system("cls");
        do{
        int rodada = 1, usuario = 1, vitoria = 0, pos, numpc;
        char simbolo = 'X';

        while(rodada <= 9 && vitoria == 0){
            if(usuario == 1){
                printf("%*.s%iº RODADA - JOGADOR %i\n", tamanhoPad(20), "", rodada, usuario);
                printarTabuleiro(Gtabuleiro);
                printf("%*.sDigite o número do espaço onde deseja colocar o %c: ", tamanhoPad(51), "", simbolo);
                scanf("%i", &pos);
                colocarPeca(pos, usuario);
            }else if(usuario == 3){
                printf("%*.s%iº RODADA - COMPUTADOR\n", tamanhoPad(21), "", rodada);
                numpc = escolherPecaRand();
                printarTabuleiro(Gtabuleiro);
                printf("%*.sO Computador escolheu posicionar uma peça na posição %d!\n", tamanhoPad(55), "", numpc);
                colocarPeca(numpc, 3);
            }
            if(usuario == 1){vitoria = checarVitoria(usuario, Gposicoes1);}
            else if(usuario == 3){vitoria = checarVitoria(usuario, Gposicoes2);}
            rodada++;
            if(usuario == 1){
                usuario += 2;
                simbolo = 'O';
            }else{
                usuario -= 2;
                simbolo = 'X';
            }
            linhaH();
            }

            printarTabuleiro(Gtabuleiro);
            printf("\n");
            if(vitoria == 1){
                salvarStats(vitoria, 2);
                printf("%*.s\aParabéns Jogador 1 você ganhou !!!\n\n", tamanhoPad(34), "");
            }else if(vitoria == 3){
                salvarStats(vitoria, 2);
                printf("%*.s\aO Computador ganhou !!!\n\n", tamanhoPad(23), "");
            }else{
                salvarStats(0, 2);
                printf("%*.s\aDeu velha ! Ninguém ganhou :(\n\n", tamanhoPad(29), "");
            }
            printf("%*.sPRESSIONE R PARA JOGAR NOVAMENTE | PRESSIONE B PARA VOLTAR AO MENU", tamanhoPad(67), "");
            printf("\n%*.s", tamanhoPad(0), "");
            do{
            GmenuResposta = getch();
            if(GmenuResposta == 'b' || GmenuResposta == 'B'){
                system("cls");
                resetarJogo(rodada, usuario, vitoria, simbolo);
                main();
            }
            }while(GmenuResposta != 'r' && GmenuResposta != 'R' && GmenuResposta != 'b' && GmenuResposta != 'B');
            system("cls");
            resetarJogo(rodada, usuario, vitoria, simbolo);
            }while(GmenuResposta == 'r' || GmenuResposta == 'R');
            break;
    case '2':
        system("cls");
        do{
        int rodada = 1, usuario = 1, vitoria = 0, pos, numpc;
        char simbolo = 'X';

        while(rodada <= 9 && vitoria == 0){
            if(usuario == 1){
                printf("%*.s%iº RODADA - JOGADOR %i\n", tamanhoPad(20), "", rodada, usuario);
                printarTabuleiro(Gtabuleiro);
                printf("%*.sDigite o número do espaço onde deseja colocar o %c: ", tamanhoPad(51), "", simbolo);
                scanf("%i", &pos);
                colocarPeca(pos, usuario);
            }else if(usuario == 3){
                printf("%*.s%iº RODADA - COMPUTADOR\n", tamanhoPad(21), "", rodada);
                numpc = escolherPecaRand();
                printarTabuleiro(Gtabuleiro);
                printf("%*.sO Computador escolheu posicionar uma peça na posição %d!\n", tamanhoPad(55), "", numpc);
                colocarPeca(numpc, 3);
            }
            if(usuario == 1){vitoria = checarVitoria(usuario, Gposicoes1);}
            else if(usuario == 3){vitoria = checarVitoria(usuario, Gposicoes2);}
            rodada++;
            if(usuario == 1){
                usuario += 2;
                simbolo = 'O';
            }else{
                usuario -= 2;
                simbolo = 'X';
            }
            linhaH();
            }

            printarTabuleiro(Gtabuleiro);
            printf("\n");
            if(vitoria == 1){
                salvarStats(vitoria, 2);
                printf("%*.s\aParabéns Jogador 1 você ganhou !!!\n\n", tamanhoPad(34), "");
            }else if(vitoria == 3){
                salvarStats(vitoria, 2);
                printf("%*.s\aO Computador ganhou !!!\n\n", tamanhoPad(23), "");
            }else{
                salvarStats(0, 2);
                printf("%*.s\aDeu velha ! Ninguém ganhou :(\n\n", tamanhoPad(29), "");
            }
            printf("%*.sPRESSIONE R PARA JOGAR NOVAMENTE | PRESSIONE B PARA VOLTAR AO MENU", tamanhoPad(67), "");
            printf("\n%*.s", tamanhoPad(0), "");
            do{
            GmenuResposta = getch();
            if(GmenuResposta == 'b' || GmenuResposta == 'B'){
                system("cls");
                resetarJogo(rodada, usuario, vitoria, simbolo);
                main();
            }
            }while(GmenuResposta != 'r' && GmenuResposta != 'R' && GmenuResposta != 'b' && GmenuResposta != 'B');
            system("cls");
            resetarJogo(rodada, usuario, vitoria, simbolo);
            }while(GmenuResposta == 'r' || GmenuResposta == 'R');
        break;
    }
}
