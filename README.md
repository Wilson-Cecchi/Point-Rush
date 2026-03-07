# Point Rush

![C](https://img.shields.io/badge/language-C-blue)
![License](https://img.shields.io/badge/license-MIT-green)

Point Rush é um jogo de terminal desenvolvido em **C** como projeto educacional.  
O objetivo do jogador é coletar todos os pontos do mapa no **menor tempo possível**, enquanto evita a corrupção que se espalha pelo cenário a cada movimento.

O projeto foi desenvolvido como **trabalho de faculdade**, com foco em praticar lógica de programação, estruturas de controle e manipulação de arquivos na linguagem C.

---

## Gameplay

O jogador deve navegar pelo mapa coletando todos os pontos enquanto evita a corrupção (`X`), que se espalha progressivamente pelo cenário.

Cada movimento realizado faz com que a corrupção avance, aumentando a dificuldade do jogo.

### Formas de terminar o jogo

**Final positivo**

- coletar todos os pontos do mapa antes do tempo acabar

**Finais negativos**

- o tempo acabar
- colidir com um obstáculo
- ficar sem movimentos possíveis
- desistir manualmente (`Q`)

A pontuação final é baseada **no tempo necessário para completar o jogo**.

Quanto **menor o tempo**, **melhor a posição no ranking**.

---

## Controles

| Tecla | Ação |
|------|------|
| W | mover para cima |
| A | mover para esquerda |
| S | mover para baixo |
| D | mover para direita |
| ↑ ↓ ← → | mover com as setas |
| Q | desistir do jogo |

---

## Funcionalidades

- jogo baseado em **terminal**
- desenvolvido na linguagem **C**
- sistema de **níveis com dificuldade progressiva**
- corrupção (`X`) que se espalha a cada movimento
- sistema de **ranking baseado em tempo**
- interface com **cores ANSI**

---

## Compilação

O jogo foi desenvolvido para **Linux e macOS**.

Compilar utilizando **GCC**:

```bash
gcc point_rush.c -o point_rush
```

---

## Execução

```bash
./point_rush
```

---

## Estrutura do projeto

```
Point-Rush
│
├── point_rush.c
├── LICENSE
└── README.md
```

---

## Objetivo do projeto

Este projeto foi desenvolvido com o objetivo de praticar:

- programação em C
- lógica de jogos
- estruturas de controle
- manipulação de arquivos
- organização de código em projetos

---

## Futuras melhorias

Possíveis evoluções do projeto:

- versão compatível com **Windows**
- melhorias visuais no terminal
- novos modos de jogo
- melhorias no sistema de ranking

---

## Autor

Wilson Klein Cecchi

GitHub  
https://github.com/Wilson-Cecchi

---

## Licença

Este projeto está licenciado sob a **MIT License**.
