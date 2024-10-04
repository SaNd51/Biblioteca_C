//comando de compilção: gcc -o sand sand.c -lpcre -Wall -Wextra -pedantic -std=c99
// ./sand
// muitas noites e dias sem dormir...
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>


// Define as setas do teclado
#define UP_ARROW 65
#define DOWN_ARROW 66
#define ENTER 10
#define BACK 127
// Códigos ANSI para cor de fundo
#define GREEN_BG "\e[48;5;2m"
#define WHITE_BG "\033[47m\033[30m"
#define BLACK_BG "\e[48;5;0m"
#define BLUE_BG "\e[48;5;20m"
#define ORANGE "\e[38;5;208m"
#define GREEN "\e[38;5;2m"
#define BLACK "\e[38;5;0m"
#define OCEAN "\e[38;5;50m"
#define RED "\e[38;5;1m"
// Limpar cores
#define RESET "\033[0m"
#define CLEAR "\033[H\033[J"
// Definição de tamanho
#define MAX_LIVROS 100 // Definição do número máximo de livros
#define MAX_LENGTH 50 // Tamanho string
#define MAX_OP 11 // Definição do número máximo de Opções
#define CURSOR_MEIO "\e["
#define ESPACO "    "

// Estrutura para armazenar informações de um livro
struct CadastroLivro {
    char nome[MAX_LENGTH];
    char autor[MAX_LENGTH];
    char genero[MAX_LENGTH];
    char sinopse[MAX_LIVROS];
    int  emprestado;
    int  num_livro;
};

struct CadastroEmprestimo
{
    char nome[MAX_LENGTH];
    char telefone[15];
    char livro_emp[5];
};

// variaveis globais
struct CadastroLivro livros[MAX_LIVROS];
struct CadastroEmprestimo pessoa[MAX_LIVROS];

const int largura = 120; // Define um valor para o tamanho da minha tela
const char *LivrosArquivo = "livros.txt",*PessoaArquivo = "emprestimos.txt", *cor, *background;
char valor[20], 
op[MAX_OP][20] = {{"Adicionar Livros"}, {"Opção de Livros"}, {"Buscar Livro"}, 
{"Emprestimos"}, {"Devolução"}, {"Temas"}, 
{"Sair"}, {"Alterar Livro"}, {"Excluir Livro"},{"Emprestar Livro"}, {"Voltar"}};
char key, topo[10][20] = {{"Nome"},{"Autor"},{"Genero"},{"Sinopse"}, {"Salvar"},{"Emprestado"},
{"Emprestimo"},{"Nome"},{"Telefone"},{"Livro"}}; 
int t = 0, p = 0, selected = 0, entrada = 0, calc = (largura / 2) + (largura % 2);

int Escolha(int selected, int num);
int printMenu();

void centralizar(char *titulo)
{
    // Calcula o número de espaços necessários para centralizar o título
    int espacos = (largura - strlen(titulo)) / 2;

    // Imprime o título centralizado
    printf("\n");
    printf(ESPACO "%*s%s%*s\n", espacos, "", titulo, espacos, "");
}

void tituloCabecalho(char *titulo)
{
    system("clear");

    // Imprime a linha superior
    printf("\n");
    printf(ESPACO "%s", cor);
    for (int i = 0; i < largura; i++)
    {
        putchar('>');
    }
    printf("\n\n");
    centralizar(titulo);
    // Imprime a linha inferior
    printf("\n");
    printf(ESPACO);
    for (int i = 0; i < largura; i++)
    {
        putchar('<');
    }
    printf("\n\n");

    printf(RESET "%s", background);
}


void desativarBufferDeEntrada()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); // Desativa entrada canônica e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void ativarBufferDeEntrada()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO; // Ativa entrada canônica e eco
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


void espacamento(char *nome) {
    // Limita o tamanho do nome a 20 caracteres para evitar overflow
    if (strlen(nome) > 20) {
        nome[20] = '\0';  // Corta o nome se for muito longo
    }

    // Imprime até 10 caracteres do nome
    for (int i = 0; i <= 10 && nome[i] != '\0'; i++) {
        printf("%c", nome[i]);
    }

    // Calcule o espaço a ser impresso após o nome
    int comp;
    if (strlen(nome) > 10) {
        printf("...");
        comp = 6; // Espaços após o nome se o nome for muito longo
    } else {
        comp = 20 - strlen(nome);
    }

    // Imprime os espaços
    for (int i = 0; i < comp; i++) {
        printf(" ");
    }

    // Imprime um espaço no final (se necessário)
    printf(" ");
}


void salvarEmp() {
    FILE *arquivo = fopen("emprestimo.dat", "wb"); // wb para escrita binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar livros.\n");
        return;
    }

    // Salva o contador de livros
    fwrite(&p, sizeof(int), 1, arquivo);

    // Salva os livros
    fwrite(pessoa, sizeof(pessoa), p, arquivo);

    fclose(arquivo);
}

void salvarLivros() {
    FILE *arquivo = fopen("livros.dat", "wb"); // wb para escrita binária
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar livros.\n");
        return;
    }

    // Salva o contador de livros
    fwrite(&t, sizeof(int), 1, arquivo);

    // Salva os livros
    fwrite(livros, sizeof(livros), t, arquivo);

    fclose(arquivo);
}


// Função para ler os emprestimos
void LerEmp() {
    FILE *arquivo = fopen("emprestimo.dat", "rb"); // rb para leitura binária
    if (arquivo == NULL) {
        // Se o arquivo não existe, inicializa o contador como 0
        printf("Arquivo de livros não encontrado. Iniciando sem livros.\n");
        t = 0;
        return;
    }

    fread(&p, sizeof(int), 1, arquivo);
    
    fread(pessoa, sizeof(pessoa), p, arquivo);

    fclose(arquivo);
}

// Função para ler os livros
void lerLivros() {
    FILE *arquivo = fopen("livros.dat", "rb"); // rb para leitura binária
    if (arquivo == NULL) {
        // Se o arquivo não existe, inicializa o contador como 0
        printf("Arquivo de livros não encontrado. Iniciando sem livros.\n");
        t = 0;
        return;
    }

    // Lê o contador de livros
    fread(&t, sizeof(int), 1, arquivo);

    // Lê os livros armazenados
    fread(livros, sizeof(livros), t, arquivo);

    fclose(arquivo);
}


int compararLivros(const void *a, const void *b) {
    const struct CadastroLivro *livroA = (const struct CadastroLivro *)a;
    const struct CadastroLivro *livroB = (const struct CadastroLivro *)b;
    int result = strcmp(livroA->nome, livroB->nome);
    if (result == 0) {
        return strcmp(livroA->autor, livroB->autor);
    }
    return result;
}

// Verifica se valor já existe
int existe()
{   
    for (int i = 0; i < t; i++)
    {
        for (int j = i+1; j < t; j++)
        {
           // Verifica se é um livro diferente do que está sendo alterado
            if (strcmp(livros[i].nome, livros[j].nome) == 0 &&
                strcmp(livros[i].autor, livros[j].autor) == 0)
            {
                return 1;
            }
        } 
    }

    return 0;
}

// Funçao para verificar espaços (gpt)
int isNotEmpty(const char *s)
{
    // Returns 1 if the string is not empty and not just spaces
    while (*s)
    {
        if (!isspace((unsigned char)*s))
        {
            return 1; // Contains non-space characters
        }
        s++;
    }
    return 0; // All characters are spaces or empty
}

// Funçao para limpar espaços(gpt)
void trim(char *s)
{
    char *start = s, *end;

    // Remove leading spaces
    while (isspace((unsigned char)*start))
        start++;

    // If the string is all spaces, return an empty string
    if (*s == '\0')
    {
        *s = '\0';
        return;
    }

    // Shift the trimmed string to the beginning
    memmove(s, start, strlen(start) + 1);

    // Find the end of the string
    end = s + strlen(s) - 1;

    // Remove trailing spaces
    while (end > s && isspace((unsigned char)*end))
        end--;

    // Null terminate the string
    *(end + 1) = '\0';
}

// Validação de campo (gpt/eu)
void validarString(char *campo, size_t tamanhoMaximo, const char *prompt)
{
    printf(CURSOR_MEIO "30C");
    while (1)
    {
        // Salva a posicao atual do cursor
        printf("\e[s");
        printf("%s:", prompt);
        sprintf(campo, "%s", "");
        if (fgets(campo, tamanhoMaximo, stdin) != NULL)
        {
            
            //Remove a nova linha que o fgets pode capturar
            campo[strcspn(campo, "\n")] = '\0';
            
            trim(campo);

            // Check if the trimmed input is not empty
            if (isNotEmpty(campo))
            {
                break;
            }
            else
            {
                printf("\e[u"); // Restaura a posicao do cursor salva anteriormente
                printf("\e[K"); // Limpa da posicao atual do cursor ate o final da linha
            }
        }
        else
        {
            centralizar("Erro na leitura da entrada. Tente novamente.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n')
                ;
        }
    }
}

// Função para validar a entrada de inteiro (alterei a função da string para inteiro)
int validarInteiro(int inicioIntervalo, int fimIntervalo)
{
    char buffer[100];
    printf(CURSOR_MEIO "%sC", valor);
    while (1)
    {
        // Salva a posicao atual do cursor
        printf("\e[s");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            
            // Remove a nova linha que o fgets pode capturar
            buffer[strcspn(buffer, "\n")] = 0;

            // Converte a string para inteiro
            char *endptr;
            entrada = strtol(buffer, &endptr, 10);

            // Verifica se a conversão foi bem-sucedida e se o valor é 0 ou 1
            if (*endptr == '\0' && endptr != buffer && (entrada >= inicioIntervalo && entrada <= fimIntervalo))
            {
                return entrada;
            }
            else
            {
                printf("\e[u"); // Restaura a posicao do cursor salva anteriormente
                printf("\e[K"); // Limpa da posicao atual do cursor ate o final da linha
            }
        }
        else
        {
            centralizar("Erro na leitura da entrada. Tente novamente.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n')
                ;
        }
    }
}


int ValidarNumero(char *campo, const char *prompt){   
    printf(CURSOR_MEIO "30C");     
    while (1)
    {
        printf("\e[s");
        printf("%s:", prompt);
        if(fgets(campo, 15, stdin)!=NULL){
            campo[strcspn(campo, "\n")] = 0;

            // Verifica se a entrada é um número inteiro
            char *endptr;
            long num = strtol(campo, &endptr, 10);

            // Verifica se a conversão foi bem-sucedida e se a string contém apenas dígitos
            if (endptr != campo && *endptr == '\0' && strlen(campo) > 9 && strlen(campo) < 15) {
                return 1;
            }
            else
            {
                centralizar("Número inválido. Tente novamente...\n");
                printf("\e[u"); // Restaura a posicao do cursor salva anteriormente
                printf("\e[K"); // Limpa da posicao atual do cursor ate o final da linha

            }
        }else{
            centralizar("Erro na leitura da entrada. Tente novamente.\n");
            // Limpa o buffer de entrada
            while (getchar() != '\n');
        }
    }
    
}


// Muda o Tema do sistema
void escolherCor()
{
    printf(RESET "%s", background);
    tituloCabecalho("Tema do sistema");
    centralizar("Escolha o Tema(0 LIGHT/ 1 DARK)");
    entrada = validarInteiro(0, 1);
    if (entrada == 0)
    {
        background = WHITE_BG; // Cor clara
        cor = RED;
    }
    else if (entrada == 1)
    {
        background = BLACK_BG; // Cor escura
        cor = ORANGE;
    }
    printf(RESET "%s", background);
    system("clear");
}

// Faz a leitura dos campos livros formatados
int lerCamposL(int n)
{
    validarString(livros[n].nome, sizeof(livros[n].nome), ESPACO "Nome");
    validarString(livros[n].autor, sizeof(livros[n].autor), ESPACO "Autor");
    validarString(livros[n].genero, sizeof(livros[n].genero), ESPACO "Gênero");
    validarString(livros[n].sinopse, sizeof(livros[n].sinopse), ESPACO "Sinopse");
    t++;
    
}
// Faz a leitura dos campos pessoa formatados
int lerCamposP(int n, int num)
{
    tituloCabecalho("Emprestarlivro");
    centralizar("Dados pessoa");
    validarString(pessoa[n].nome, sizeof(pessoa[n].nome), ESPACO "Nome completo");
    ValidarNumero(pessoa[n].telefone, ESPACO "Número de telefone");
    snprintf(pessoa[n].livro_emp, sizeof(pessoa[n].livro_emp), "%d", livros[num].num_livro);
    livros[num].emprestado = 1;
    p++;
    salvarEmp();
    tituloCabecalho("Emprestarlivro");
    centralizar("Os dados foram salvo com sucesso!\n");
}

void printLivro(int livro){
    printf(ESPACO "| Nome:%s \n", livros[livro].nome);
    printf(ESPACO "| Autor:%s \n", livros[livro].autor);
    printf(ESPACO "| Gênero:%s \n", livros[livro].genero);
    printf(ESPACO "| Sinopse:%s\n", livros[livro].sinopse);
    printf(ESPACO "| Livro número: %d", livros[livro].num_livro);
    printf("\n\n");

}

void opcaoLivro(int num)
{
    selected = 8;
    char titulo[20] = "Seleção do Livro";
    tituloCabecalho(titulo);
    while (1)
    {
        printf(CLEAR);
        tituloCabecalho(titulo);
        
        centralizar("livro\n");
        printLivro(num);
        
        for (int i = 8; i <= MAX_OP; i++)
        {
            printf("%s", background);
            if (selected == i)
            {
                printf(ESPACO "%s -> %d. %s <- \n" RESET, cor, i - 7, op[i - 1]);
                printf("%s", background);
            }
            else
                printf(ESPACO "%d. %s \n", i - 7, op[i - 1]);
        }
        desativarBufferDeEntrada();
        key = getchar();
        ativarBufferDeEntrada();
        switch (key)
        {
            case UP_ARROW:
                selected--;
                if (selected < 8)
                    selected = MAX_OP;
                break;
            case DOWN_ARROW:
                selected++;
                if (selected > MAX_OP)
                    selected = 8;
                break;
            case ENTER:
                Escolha(selected, num);
                break;
        }
    }
}

int TrocarCampos(int selected, int num){
    switch (selected)
    {
    case 0:
        validarString(livros[num].nome, sizeof(livros[num].nome), ESPACO "Nome");
        break;
    case 1:
        validarString(livros[num].autor, sizeof(livros[num].autor), ESPACO "Autor");
        break;
    case 2:
        validarString(livros[num].genero, sizeof(livros[num].genero), ESPACO "Gênero");
        break;
    case 3:
        validarString(livros[num].sinopse, sizeof(livros[num].sinopse), ESPACO "Sinopse");
        break;
    }
    
}

// Adiciona valores dentro da struc livros
void Adicionar()
{
    char titulo[20] = "Adicionar Livro";
    
    tituloCabecalho(titulo);

    if (t >= MAX_LIVROS)
    {
        centralizar("Limite de livros atingido!\n");
        return;
    }

    while (1)
    {
        centralizar("Digite 1 para adicionar ou 0 para voltar ao menu...");
        entrada = validarInteiro(0, 1);
        if (entrada == 0)
        {
            tituloCabecalho(titulo);
            break;
        }
        if (entrada == 1)
        {
            tituloCabecalho(titulo);
            // Verifique se o livro já existe antes de adicionar
            lerCamposL(t);

            if (existe())
            {
                centralizar("Livro já existente. Tente novamente com valores diferentes.");
                t--;
            }
            else
            {
                livros[t].emprestado = 0;
                livros[t].num_livro = t;
                tituloCabecalho(titulo);
                centralizar("Livro salvo com sucesso!\n");
            }
        }
        //printf("%d",t);
    }
    centralizar("Pressione Enter para voltar ao menu principal...");
    desativarBufferDeEntrada();
    getchar();
}

// Função para alterar valores na struct
void Alterar(int num)
{
    char titulo[20] = "Alterar Livro";
    int selected = 0, valor=1;
    tituloCabecalho(titulo);
    
    while (1)
    {
        tituloCabecalho(titulo);
        printLivro(num);

        if(existe()){
            centralizar("Inválido!\n");
            centralizar("Nome e autor já cadastrados. Troque os campos!\n");
        }else
            centralizar("Escolha o campo para alterar ou salve as informações\n");

        for (int i = 0; i <= 4; i++)
            {
                printf("%s", background);
                if (selected == i)
                {
                    printf(ESPACO"%s-> %s <-\n"RESET, cor , topo[i]);
                    
                    printf("%s", background);
                }
                else
                    printf(ESPACO"%s\n", topo[i]);
            }

        desativarBufferDeEntrada();
        key = getchar();
        ativarBufferDeEntrada();    
        switch (key)
        {
            case UP_ARROW:
                selected--;
                if (selected < 0)
                    selected = 4;
                break;
            case DOWN_ARROW:
                selected++;
                if (selected > 4)
                    selected = 0;
                break;
            case ENTER:
                if(selected==4){
                    (!existe())?opcaoLivro(num):tituloCabecalho(titulo);
                }else 
                    TrocarCampos(selected, num);
                break;
        }
        
    }

    // Verifique se o livro alterado já existe antes de salvar as alterações    
    centralizar("Pressione Enter para voltar...\n");
    desativarBufferDeEntrada();
    getchar();
    
}

void listarEmp(){
    tituloCabecalho("Lista de Emprestimo");
    if (p == 0)
    {
        centralizar("Nenhum emprestimo!\n");
    }
    else
    {
        for (int i = 0; i < ((largura - 60) / 2); i++){putchar(' ');} // centraliza ao meio
        for (int i = 6; i < 10; i++){espacamento(topo[i]);} // para o numero de campos no vetor
        printf("\n");
        for (int i = 0; i < p; i++)
            {
                for (int i = 0; i < ((largura - 60) / 2); i++){putchar(' ');} // centraliza ao meio 
                printf("%d. ",i+1);
                for (int i = 0; i < 18; i++)
                    putchar(' ');
                espacamento(pessoa[i].nome);
                espacamento(pessoa[i].telefone);
                for (int j = 0; j < t; j++){if(livros[j].num_livro == atoi(pessoa[i].livro_emp)){espacamento(livros[j].nome);}}
                printf("\n");
            }
        
        printf("\n\n");      
          
    }
    centralizar("Pressione Enter para voltar ao menu principal...");
    desativarBufferDeEntrada();
    getchar();
}

void EmprestarLivro(int num){
    int entrada, teste;
    tituloCabecalho("Emprestar livro");
    if (p >= MAX_LIVROS)
    {
        centralizar("Limite de Emprestimos atingido!\n");
        return;
    }
    else{
        tituloCabecalho("Emprestar livro");
        //for (int i = 0; i < t; i++){if(livros[i].num_livro == num){teste = i;}}
        if(!livros[num].emprestado){
            centralizar("Digite 1 para adicionar ou 0 para voltar ao menu...");
            entrada = validarInteiro(0, 1);
            (!entrada)?tituloCabecalho("Emprestar livro"):lerCamposP(p, num);                                 
        }else
            centralizar("Livro já emprestado!\n");
    }   
    centralizar("Pressione Enter para voltar ao menu principal...");
    desativarBufferDeEntrada();
    getchar();
}

void devolverLivro(){
    int num, n;

    tituloCabecalho("Devolução do livro");

    if (p == 0)
    {
        centralizar("Nenhum emprestimo!\n");
    }else{
        centralizar("Informe o número do emprestimo ou 0 para voltar...");
        
        num = validarInteiro(0,p) - 1; 
        if(num<0){
            printMenu();
        }else{

            tituloCabecalho("Devolução do livro");
            // printf(ESPACO "%s devolveu o livro: %s\n", pessoa[num].nome, livros[num].nome);

            for (int i = 0; i < t; i++){if(livros[i].num_livro == atoi(pessoa[num].livro_emp)){livros[i].emprestado = 0;}}
            for (int i = num; i < p; i++) {

                pessoa[i] = pessoa[i + 1]; // Mova os elementos
            }
            p--; 
            
            centralizar("Livro devolvido com sucesso!\n"); 
        }      

    }      

            
    centralizar("Pressione Enter para voltar ao menu principal...");
    desativarBufferDeEntrada();
    getchar();
}

// Excluir um valor dentro da struct
void Excluir(int num)
{
    char titulo[20] = "Excluir Livro";
    tituloCabecalho(titulo);
    // printf("Nome: %s\n",livros[num].nome);
    
    // Remove o emprestimo
    if(livros[num].emprestado){
        for (int i = 0; i < p; i++)
        {
            if(livros[num].num_livro== atoi(pessoa[i].livro_emp))
                pessoa[i] = pessoa[i + 1]; // Substitui o valor pelo seguinte
        }
        p--; // Reduz o contador de pessoa
    }

    // Remove o livro selecionado
    for (int i = num; i < t - 1; i++)
    {
        livros[i] = livros[i + 1]; // Substitui o valor pelo seguinte
    }
    t--; // Reduz o contador de livros

    

    tituloCabecalho(titulo);
    centralizar("Excluído com sucesso!\n");

       
    centralizar("Pressione Enter para voltar ao menu principal...\n");
    desativarBufferDeEntrada();
    if (getchar())
        printMenu();
}

// Buscar valor pelo índice nome
int Buscar()
{
    char nome[20], titulo[20] = "Busca pelo Livro";
    int encontrado = 0, livro;
    tituloCabecalho(titulo);
    if (t == 0)
    {
        centralizar("Nenhum livro cadastrado.\n");
    }
    else
    {
        centralizar("Digite o nome do Livro\n");
        validarString(nome, sizeof(nome), "Nome");
        tituloCabecalho(titulo);
        printf(ESPACO " # Resultado da busca: \n");
        for (int i = 0; i < t; i++)
        {
            if (strcmp(livros[i].nome, nome) == 0)
            {
                encontrado = 1;
                printLivro(i);
            }
        }

        if (!encontrado)
        {
            tituloCabecalho(titulo);
            centralizar("livro não encontrado!\n");
        }
        
    }
    centralizar("Pressione Enter para voltar ao menu principal...");
    desativarBufferDeEntrada();
    getchar();
}

// lista de Livros cadastrados
void ListarLivros()
{
    selected = 0;
    int tam_print = 60;
    char titulo[20] = "Lista de Livros";
    tituloCabecalho(titulo);
    if (t == 0)
    {
        centralizar("Nenhum livro cadastrado!\n");
    }
    else
    {
        while (1)
        {
            printf(CLEAR);
            tituloCabecalho(titulo);
            centralizar("Use as setas para navegar e Enter para selecionar\n");

            qsort(livros, t, sizeof(struct CadastroLivro), compararLivros); // ordena o vetor de acordo com o nome
            for (int i = 0; i < ((largura - tam_print) / 2); i++){putchar(' ');} // centraliza ao meio
            for (int i = 0; i < 6; i++)
                if(i == 5 || i < 3){espacamento(topo[i]);} // para o numero de campos no vetor
            printf("\n");
            for (int i = 0; i < t; i++)
            {
                for (int i = 0; i < ((largura - 5*20)/2); i++)
                    putchar(' '); // centraliza ao meio
                if (selected == i)
                {
                    printf(GREEN_BG " ->");
                    for (int i = 0; i < 17; i++)
                        putchar(' ');
                    espacamento(livros[i].nome);
                    espacamento(livros[i].autor);
                    espacamento(livros[i].genero);
                    (livros[i].emprestado)? espacamento("sim"): espacamento("não");
                    printf("<- " RESET "\n%s", background);
                }
                else
                {
                    printf("   ");
                    for (int i = 0; i < 17; i++) 
                        putchar(' ');
                    espacamento(livros[i].nome);
                    espacamento(livros[i].autor);
                    espacamento(livros[i].genero);
                    (livros[i].emprestado)? espacamento("sim"): espacamento("não");
                    printf("\n");
                }
            }
            printf("\n\n");
            centralizar("Pressione BACK SPACE para voltar ao menu principal...\n");

            desativarBufferDeEntrada();
            key = getchar();
            ativarBufferDeEntrada();

            switch (key)
            {
                case UP_ARROW:
                    selected--;
                    if (selected < 0)
                        selected = t-1;
                    break;
                case DOWN_ARROW:
                    selected++;
                    if (selected > t-1)
                        selected = 0;
                    break;
                case ENTER:
                    opcaoLivro(selected);
                    break;
                case BACK:
                    printMenu();
                    break;
            }
            
        }
    }
    centralizar("Pressione Enter para voltar ao menu principal...");
    desativarBufferDeEntrada();
    getchar();
}



int Escolha(int selected, int num)
{
    system("clear");
    ativarBufferDeEntrada();
    salvarLivros();
    salvarEmp();
    switch (selected)
    {
    case 1:
        Adicionar();
        break;
    case 2:
        ListarLivros();
        break;
    case 3:
        Buscar();
        break;
    case 4:
        listarEmp();
        break;
    case 5:
        devolverLivro();
        break;
    case 6:
        escolherCor();
        break;
    case 7:
        printf(RESET CLEAR);
        exit(0);
        break;
    case 8:
        Alterar(num);
        break;
    case 9:
        Excluir(num);
        break;
    case 10:
        EmprestarLivro(num);
        break;
    case 11:
        ListarLivros();
        break;
    }
    return selected;

    
}


int printMenu()
{
    selected = 1;
    int tam = MAX_OP - 4;
    printf(CLEAR RESET);
    system("clear");
    printf("%s", background);
    
    while (1)
    {
        printf(CLEAR);
        tituloCabecalho("BIBLIOTECA");
        centralizar("Use as setas para navegar e Enter para selecionar:\n");
        for (int i = 1; i <= tam; i++) // Muda de acordo com as setas
        {
            printf("%s", background);
            if (selected == i)
                printf(ESPACO "%s -> %d. %s <- \n" RESET, cor, i, op[i - 1]);
            else
                printf(ESPACO "%d. %s \n", i, op[i - 1]);
            printf("%s", background);
        }
        desativarBufferDeEntrada();
        key = getchar();
        ativarBufferDeEntrada();
        switch (key)
        {
            case UP_ARROW:
                selected--;
                if (selected < 1)
                    selected = tam;
                break;
            case DOWN_ARROW:
                selected++;
                if (selected > tam)
                    selected = 1;
                break;
            case ENTER:
                Escolha(selected, 0);
                break;
            case BACK:
                printMenu();
                break;
        }
    }
    
    return 0;
}

int main()
{
    lerLivros();
    LerEmp();
    sprintf(valor, "%d", calc);
    background = BLACK_BG;
    cor = ORANGE;
    while (printMenu())
        ;
    
    return 0;
}