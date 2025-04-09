#pragma once
#ifndef PESSOA_FORMULARIO_H
#define PESSOA_FORMULARIO_H

#include <iostream>
#include <list>
#include <string>
#include <regex>
#include <ctime>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <sstream>

struct Pessoa {
    std::string nomeCompleto;
    std::string email;
    std::string cpf;
    std::string telefone;
    std::string dataNascimento;
    std::string senha;
    std::string dataCadastro;
};

class Form {
public:
    bool validarCPF(const std::string& cpf);
    bool validarEmail(const std::string& email);
    bool validarTelefone(const std::string& telefone);
    bool validarDataNascimento(const std::string& data);
    bool validarSenha(const std::string& senha);
    static std::string dataAtual();

    void criarFormulario();
    void stressTest(int quantidade);

};

std::list<Pessoa> m_bancoDeDados;
std::unordered_set<std::string> cpfs, emails, telefones;

#endif
