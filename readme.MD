# Termo - Jogo de Palavras em GTK

Este é um clone do famoso jogo de palavras "Termo", desenvolvido em C utilizando a biblioteca GTK para a interface gráfica. O objetivo do jogo é adivinhar a palavra correta em até 6 tentativas.

---

## ⚠️ Observação

Este projeto ainda está em desenvolvimento. Algumas funcionalidades podem não estar completas ou podem apresentar bugs. Contribuições são bem-vindas!

---

## 🚀 Funcionalidades

- Interface gráfica amigável com GTK.
- Teclado virtual que exibe as letras usadas e suas cores:
  - **Verde**: Letra correta na posição correta.
  - **Amarelo**: Letra correta na posição errada.
  - **Cinza**: Letra incorreta.
- Mensagens de vitória ou derrota com estilo visual consistente.
- Suporte a dicionários personalizados para palavras.

---

## 🛠️ Pré-requisitos

Certifique-se de ter os seguintes itens instalados no seu sistema:

- **GTK 3** ou superior.
- Compilador **GCC** ou equivalente.

No Ubuntu/Debian, você pode instalar o GTK com:
```bash
sudo apt-get install libgtk-3-dev
```

---

## 📦 Como Compilar e Executar

1. Clone este repositório:
   ```bash
   git clone https://github.com/marcio-guimaraes/termoo.git
   cd termo
   ```

2. Compile o código:
   ```bash
   gcc main.c termo.c -o game `pkg-config --cflags --libs gtk+-3.0`
   ```

3. Execute o jogo:
   ```bash
   ./game
   ```

---

## 📚 Como Jogar

1. Digite uma palavra de 5 letras no campo de entrada.
2. Clique no botão **"Enviar"** ou pressione **Enter**.
3. Observe as cores no teclado virtual:
   - **Verde**: Letra correta na posição correta.
   - **Amarelo**: Letra correta na posição errada.
   - **Cinza**: Letra incorreta.
4. Continue tentando até acertar a palavra ou esgotar as 6 tentativas.

---

## 🖌️ Personalização

### Alterar o Dicionário de Palavras
O jogo utiliza um arquivo chamado `dicionario.txt` para carregar as palavras. Para personalizar o dicionário:
1. Substitua o conteúdo do arquivo `dicionario.txt` por palavras de sua escolha.
2. Certifique-se de que cada palavra tenha exatamente 5 letras e esteja em uma linha separada.

---

## 🧑‍💻 Contribuindo

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou enviar pull requests.

---

## 📜 Licença

Este projeto está licenciado sob a licença **MIT**. Consulte o arquivo [LICENSE](LICENSE) para mais detalhes.

---

## ❤️ Agradecimentos

- Inspirado no jogo original "Termo".
- Desenvolvido com a biblioteca GTK para interfaces gráficas.
