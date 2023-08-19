#include <iostream>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include <algorithm>

using std::string, std::vector, std::cout, std::endl, std::getline, std::stringstream, std::ifstream, std::cerr, std::exit, std::cin;

namespace fs = std::filesystem;

class Sala {
    int id_sala;
    string bloco;
    string nome;
    int capacidade;
    string tipo;
public:
    // Construtor da classe sala
    Sala() {
    }
    Sala(int id, string b, string n, string c, string t) {
        id_sala = id;
        bloco = b;
        nome = n;
        capacidade = std::stoi(c);
        tipo = t;
    }
    void print_info() {
        cout << "**SALA**\n"
        << "ID: " << id_sala << "\n"
        << "Bloco: " << bloco << "\n"
        << "Nome: " << nome << "\n"
        << "Capacidade: " << capacidade << "\n"
        << "Tipo: " << tipo << endl;
    }
    int get_cap(){
        return capacidade;
    }
};

class Turma {
    int id_turma;
    string disciplina;
    string turma;
    string horario;
    int num_alunos;
    string preferencia;
public:
    Turma(int id, string d, string t, string h, int n, string p) {
        id_turma = id;
        disciplina = d;
        turma = t;
        horario = h;
        num_alunos = n;
        preferencia = p;
    }
    string get_horario(){
        return horario;
    }
    void print_info() {
        cout
        << "// Turma " << id_turma << " //\n"
        << "Disciplina: " << disciplina << "\n"
        << "Turma: " << turma << "\n"
        << "Horário: " << horario << "\n"
        << "Num. alunos: " << num_alunos << "\n"
        << "Preferência: " << preferencia << endl;
    }
};

class TurmaAlocada {
    int id_turma;
    string disciplina;
    int num_alunos;
    Sala* sala;
public:
    TurmaAlocada(int id, string d, int n, Sala* s) {
        id_turma = id;
        disciplina = d;
        num_alunos = n;
        sala = s;
    }
    
    int get_num_alunos(){
        return num_alunos;
    }
    int get_cap_sala(){
        return sala->get_cap();
    }
    string get_disciplina(){
        return disciplina;
    }
    Sala* get_sala(){
        return sala;
    }
    void print_info() {
        cout << std::fixed << std::setprecision(2)
        << "// Turma " << id_turma << " //\n"
        << "Disciplina: " << disciplina << "\n"
        << "Num. alunos: " << num_alunos << "\n" 
        << "Ocupação: " << ((float)this->get_num_alunos() / this->get_cap_sala())*100 << "%" << endl;
        sala->print_info();
    }
};

class Instituicao {
    string nome;
    vector<Sala> salas;
    vector<Turma*> turmas;
    //vector<TurmaAlocada> turmas_alocadas;
public:
    Instituicao(string n, vector<Sala> s, vector<Turma*> t) {
        nome = n;
        salas = s;
        turmas = t;
    }
    void print_info() {
        cout << "Instituição: " << nome << endl;
        cout << "Salas: " << endl;
        for(int i = 0; i < salas.size(); i++){
            salas[i].print_info();
            cout << endl;
        }
        cout << "Turmas: " << endl;
        for(int i = 0; i < turmas.size(); i++){
            turmas[i]->print_info();
            cout << endl;
        }
    }
    vector<string> get_horarios(){
        vector<string> horarios;
        for(int i = 0; i < turmas.size(); i++){
            // checar se o horario já está no vetor
            bool horarioJaExiste = false;
            for(int j = 0; j < horarios.size(); j++){
                if(turmas[i]->get_horario() == horarios[j]){
                    horarioJaExiste = true;
                    break;
                }
            }
            if(!horarioJaExiste){
                horarios.push_back(turmas[i]->get_horario());
            }
        }
        return horarios;
    }
};



int main(void){

    // ENCONTRA ARQUIVOS CSV NA PASTA
    vector<string> csvFiles;

    fs::path currentDir = fs::current_path();

    // IDEIA: FAZER COM QUE O VETOR DE ARQUIVOS ENCONTRADOS SEJA ORDENADO POR NOME
    for (const auto & entry : fs::directory_iterator(currentDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            csvFiles.push_back(entry.path().filename().string());
        }
    }

    if(csvFiles.empty()){
        cout << "Nenhum arquivo csv encontrado na pasta" << endl;
        exit(1);
    }else{
        std::sort(csvFiles.begin(), csvFiles.end());
    }

    cout << "// Bem vindo ao sistema de alocação de salas //" << endl;
    cout << "\nVocê pode começar lendo o arquivo de salas disponíveis." << endl;

    cout << "\033[1m" << "\nLEMBRETE: O arquivo de salas deve estar no formato: 'bloco,nome,capacidade,tipo'" << "\033[0m" << endl;

    char c;
    cout << "Pressione enter para continuar" << endl;
    cin.get(c);


    
    cout << "Aqui estão os arquivos csv encontrados na pasta:" << endl;
    for(size_t i = 0; i < csvFiles.size(); ++i){
        cout << "(" << i+1 << ") - '" << csvFiles[i] << "'" << endl;
    }


    // Solicia ao usuário que escolha um arquivo
    cout << "Digite o número do arquivo que deseja abrir: ";
    int escolha;
    cin >> escolha;

    // Verifica se a escolha é válida
    if(escolha >= 1 && escolha <= csvFiles.size()){
        cout << "\nVocê escolheu o arquivo: " << csvFiles[escolha - 1] << endl;
    } else {
        cout << "\nEscolha inválida" << endl;
        exit(1);
    }
    string arquivoEscolhido = csvFiles[escolha - 1];

    // Lê o arquivo csv de salas
    std::ifstream file(arquivoEscolhido);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << arquivoEscolhido << std::endl;
        exit(1);
    }
    vector<Sala> salas;

    string header;
    std::getline(file, header); // Descarta 1° Linha
    header.clear();

    string linha;
    int i = 0;
    while (std::getline(file, linha)) {
        std::stringstream ss(linha);
        string c1, c2, c3, c4;

        std::getline(ss, c1, ',');
        std::getline(ss, c2, ',');
        std::getline(ss, c3, ',');
        std::getline(ss, c4, ',');
        // cout << c1 << " " << c2 << " " << c3 << " " << c4 << endl;

        salas.push_back(Sala(i+1, c1, c2, c3, c4));
        i++;
    }
    file.close();
    linha.clear();

    cout << "Salas alocadas com sucesso! Deseja ver as salas alocadas? (s/n)" << endl;
    char resposta;
    cin >> resposta;

    if(resposta == 's'){
        for(int i = 0; i < salas.size(); i++){
            salas[i].print_info();
            cout << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }else{
        cout << endl;
    }

    // Remove o arquivo csv escolhido da lista
    csvFiles.erase(csvFiles.begin() + escolha - 1);

    // TURMAS

    cout << "Agora você pode ler os arquivos de turmas." << endl;
    cout << "\033[1mLEMBRETE: O arquivo de turmas deve estar no formato: 'id,disciplina,turma,horario,num_alunos,preferencia'\033[0m" << endl;

    cout << endl;

    vector<Turma> turmas;

    while(!csvFiles.empty() || escolha == -1 ){

        cout << "Aqui estão os arquivos csv encontrados na pasta:" << endl;
        for(size_t i = 0; i < csvFiles.size(); ++i){
            cout << "(" << i+1 << ") - '" << csvFiles[i] << "'" << endl;
        }

        // Solicia ao usuário que escolha um arquivo
        cout << "Digite o número do arquivo que deseja abrir: ";
        cin >> escolha;

        // Verifica se a escolha é válida
        if(escolha >= 1 && escolha <= csvFiles.size()){
            cout << "\nVocê escolheu o arquivo: " << csvFiles[escolha - 1] << endl;
        } else if(escolha == -1){
            cout << "\nSaindo..." << endl;
            break;
        } else {
            cout << "\nEscolha inválida" << endl;
        }
        arquivoEscolhido = csvFiles[escolha - 1];
        
        ifstream file2(arquivoEscolhido);
        if (!file2.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << arquivoEscolhido << endl;
            exit(1);
        }

        getline(file2, header); // Descarta 1° Linha
        header.clear();

        while (getline(file2, linha)){
            stringstream ss(linha);
            string c1, c2, c3, c4, c5, c6;

            getline(ss, c1, ','); // id
            getline(ss, c2, ','); // disciplina
            getline(ss, c3, ','); // turma
            getline(ss, c4, ','); // horario
            getline(ss, c5, ','); // num_alunos
            getline(ss, c6, ','); // preferencia
            // cout << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << " " << c6 << endl;

            turmas.push_back(Turma(std::stoi(c1), c2, c3, c4, std::stoi(c5), c6));
        }
        file2.close();
        linha.clear();
        csvFiles.erase(csvFiles.begin() + escolha - 1);

        cout << "Turmas alocadas com sucesso! Deseja ver as turmas alocadas? (s/n)" << endl;
        cin >> resposta;

        if(resposta == 's'){
            for(int i = 0; i < turmas.size(); i++){
                turmas[i].print_info();
                cout << endl;
            }
        }else{
            cout << endl;
        }
    }

    vector<Turma*> turmas_geral;

    // Adiciona todas as turmas lidas no vetor de turmas da instituição
    for(int i = 0; i < turmas.size(); i++){
        turmas_geral.push_back(&turmas[i]);
    }
    // Instituição
    Instituicao instituicao("UFRN", salas, turmas_geral);

    // instituicao.print_info();
    cout << "Horários lidos: " << endl;
    for(const string& horario : instituicao.get_horarios()){
        cout << horario << endl;
    }

    // ALOCAÇÃO



    return 0;
}