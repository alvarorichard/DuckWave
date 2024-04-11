<p align="center">
  <img src="https://i.imgur.com/Ex4LsbU.png" alt="Imagem logo" />
</p>

![GitHub license](https://img.shields.io/github/license/alvarorichard/DuckWave)
![GitHub languages top](https://img.shields.io/github/languages/top/alvarorichard/DuckWave)
![GitHub last commit](https://img.shields.io/github/last-commit/alvarorichard/DuckWave)

DUCKWAVE é um reprodutor de áudio simples, escrito em C, projetado para simplicidade e facilidade de uso. Ele suporta a reprodução de vários formatos de áudio e oferece controles básicos como reproduzir, pausar e parar. O projeto é construído com dependências mínimas e focado em fornecer uma experiência amigável ao usuário para reproduzir arquivos de áudio a partir do terminal.


 ## Instalação


#### Pré-requisitos

Certifique-se de que você tenha as seguintes bibliotecas instaladas:

* `libmpg123`
* `libao`
* `libavcodec`
* `libavformat`

Em sistemas Debian/Ubuntu, você pode instalar estas usando:

```bash
sudo apt-get install libmpg123-dev libao-dev libavcodec-dev libavformat-dev
```
### Compiling the Code
```bash
clang main.c playmp3.c -o playmp3 -lmpg123 -lao -Bstatic                     
```
### Compiling the Code with the Script
```bash
chmod +x install.sh
```
 Execute o script:
```bash
./install.sh
```

Isso criará um executável chamado mp3player no seu diretório atual.

## Uso

1. Adicione seu arquivo mp3 ao diretório do projeto.
2. Na função main() do main.c, substitua "add you music here .mp3" pelo nome do seu arquivo mp3.
3. Compile o código.
4. Execute o executável:

```C
./mp3player sua musica.mp3
```
>[!IMPORTANTE]
> Observe que este projeto ainda está em desenvolvimento e pode estar sujeito a alterações e melhorias.


## Contribuindo
Contribuições para este projeto são bem-vindas. Por favor, siga estes passos para contribuir:

1. Faça um fork do repositório.
2. Crie uma nova branch para sua funcionalidade ou correção de bug.
3. Faça o commit das suas alterações.
4. Faça o push para a branch.
5. Envie um pull request.

<p align="center">
  <img src="https://i.imgur.com/5nbPY1g.png" alt="Imagem logo" style="height: 100px;"/>
</p>

