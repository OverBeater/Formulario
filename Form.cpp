#pragma once
#include "Form.h"
#include <chrono>

using namespace std;
using namespace chrono;

mutex db_mutex;

bool Form::validarCPF(const string& cpf)
{
    if (!regex_match(cpf, regex("\\d{11}")))
        return false;
    if (all_of(cpf.begin(), cpf.end(), [&](char c) { return c == cpf[0]; }))
        return false;
    int soma1 = 0;
    for (int i = 0; i < 9; ++i)
        soma1 += (cpf[i] - '0') * (10 - i);
    int dig1 = soma1 % 11;
    dig1 = (dig1 < 2) ? 0 : 11 - dig1;
    int soma2 = 0;
    for (int i = 0; i < 10; ++i)
        soma2 += (cpf[i] - '0') * (11 - i);
    int dig2 = soma2 % 11;
    dig2 = (dig2 < 2) ? 0 : 11 - dig2;
    return (cpf[9] - '0' == dig1) && (cpf[10] - '0' == dig2);
}

bool Form::validarEmail(const string& email)
{
    return regex_match(email, regex("^.+@.+\\.com$"));
}

bool Form::validarTelefone(const string& tel)
{
    return regex_match(tel, regex("\\d{11}"));
}

bool Form::validarDataNascimento(const string& data)
{
    int d, m, a;
    if (sscanf_s(data.c_str(), "%d/%d/%d", &d, &m, &a) != 3) return false;
    if (a < 1900) return false;

    time_t t = time(nullptr);
    tm now = {};
    localtime_s(&now, &t);
    int idade = now.tm_year + 1900 - a;
    if ((now.tm_mon + 1 < m) || (now.tm_mon + 1 == m && now.tm_mday < d)) idade--;
    return idade >= 18;
}

bool Form::validarSenha(const string& senha)
{
    bool ok = senha.length() >= 8 &&
        regex_search(senha, regex("[A-Z]")) &&
        regex_search(senha, regex("[a-z]")) &&
        regex_search(senha, regex("[0-9]"));

    if (!ok)
        cout << "Por favor insira no mínimo 1 letra maiuscula e no mínimo 1 número!";

    return ok;
}

string Form::dataAtual()
{
    time_t t = time(nullptr);
    tm now = {};
    localtime_s(&now, &t);
    char buffer[11];
    strftime(buffer, 11, "%d/%m/%Y", &now);
    return string(buffer);
}

void Form::criarFormulario()
{
    Pessoa regPessoa;
    char nomeBuffer[126];
    char emailBuffer[256];
    char cpfBuffer[64];
    char telefoneBuffer[64];
    char nascimentoBuffer[64];
    char senhaBuffer[128];
    char senhaConfirmaBuffer[128];

    cout << "Nome Completo: ";
    cin.getline(nomeBuffer, 126);
    regPessoa.nomeCompleto = nomeBuffer;

    do
    {
        cout << "Email: ";
        cin.getline(emailBuffer, 256);
        regPessoa.email = emailBuffer;
    } while (!validarEmail(regPessoa.email) || emails.count(regPessoa.email));

    do
    {
        cout << "CPF (somente numeros): ";
        cin.getline(cpfBuffer, 64);
        regPessoa.cpf = cpfBuffer;
    } while (!validarCPF(regPessoa.cpf) || cpfs.count(regPessoa.cpf));

    do
    {
        cout << "Telefone (com DDD, ex: 11999999999): ";
        cin.getline(telefoneBuffer, 64);
        regPessoa.telefone = telefoneBuffer;
    } while (!validarTelefone(regPessoa.telefone) || telefones.count(regPessoa.telefone));

    do
    {
        cout << "Data de Nascimento (dd/mm/yyyy): ";
        cin.getline(nascimentoBuffer, 64);
        regPessoa.dataNascimento = nascimentoBuffer;
    } while (!validarDataNascimento(regPessoa.dataNascimento));

    do
    {
        cout << "Senha: ";
        cin.getline(senhaBuffer, 128);
        cout << "Confirmar Senha: ";
        cin.getline(senhaConfirmaBuffer, 128);
        regPessoa.senha = senhaBuffer;
    } while (!validarSenha(regPessoa.senha) || regPessoa.senha != senhaConfirmaBuffer);

    regPessoa.dataCadastro = dataAtual();

    lock_guard<mutex> lock(db_mutex);
    m_bancoDeDados.push_back(regPessoa);
    cpfs.insert(regPessoa.cpf);
    emails.insert(regPessoa.email);
    telefones.insert(regPessoa.telefone);

    cout << "\nCadastro realizado com sucesso!\n";
}

void Form::stressTest(int quantidade)
{
    auto start = high_resolution_clock::now();

    vector<thread> threads;
    threads.reserve(quantidade);

    for (int i = 0; i < quantidade; ++i) {
        threads.emplace_back([i]() {
            Pessoa regPessoa = {
                "Teste Nome",
                "teste" + to_string(i) + "@email.com",
                to_string(10000000000 + i),
                "11" + to_string(100000000 + i),
                "01/01/1990",
                "SenhaF0rte",
                dataAtual()
            };

            lock_guard<mutex> lock(db_mutex);
            if (!cpfs.count(regPessoa.cpf) && !emails.count(regPessoa.email) && !telefones.count(regPessoa.telefone)) {
                m_bancoDeDados.push_back(regPessoa);
                cpfs.insert(regPessoa.cpf);
                emails.insert(regPessoa.email);
                telefones.insert(regPessoa.telefone);
            }
            });
    }

    for (auto& th : threads)
        th.join();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "\nStress test concluido com " << m_bancoDeDados.size() << " cadastros.\n";
    cout << "Tempo decorrido: " << duration.count() << " ms\n";
}

int main()
{
    Form app;
    int opcao;
    do {
        cout << "\n1. Criar Formulario\n2. Stress Test\n3. Sair\nOpcao: ";
        cin >> opcao;
        cin.ignore();
        if (opcao == 1) app.criarFormulario();
        else if (opcao == 2) {
            int qtd;
            cout << "Quantidade de testes: ";
            cin >> qtd;
            cin.ignore();
            app.stressTest(qtd);
        }
    } while (opcao != 3);

    return 0;
}
