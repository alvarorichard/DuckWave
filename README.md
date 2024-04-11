

<p align="center">
  <img src="https://i.imgur.com/Ex4LsbU.png" alt="Imagem logo" />
</p>

![GitHub license](https://img.shields.io/github/license/alvarorichard/DuckWave)
![GitHub languages top](https://img.shields.io/github/languages/top/alvarorichard/DuckWave)
![GitHub last commit](https://img.shields.io/github/last-commit/alvarorichard/DuckWave)

DUCKWAVE is a straightforward audio player written in C, designed for simplicity and ease of use. It supports playing multiple audio formats and offers basic controls such as play, pause, and stop. The project is built with minimal dependencies and is focused on providing a user-friendly experience for playing audio files from the terminal.


 ## Installation

#### Prerequisites

Ensure you have the following libraries installed:

* `libmpg123`
* `libao`
* `libavcodec`
* `libavformat`

On Debian/Ubuntu systems, you can install these using:

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
 Run the script:
```bash
./install.sh
```


This will create an executable called mp3player in your current directory.

## Usage

1. Add your mp3 file to the project directory.
2. In the main() function of main.c, replace "add you music here .mp3" with your mp3 file name.
3. Compile the code.
4. Run the executable:
```C
./mp3player your music.mp3
```
>[!IMPORTANT]
> Please note that this project is still under development and may be subject to changes and improvements.


## Contributing
Contributions to this project are welcome. Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Commit your changes.
4. Push to the branch.
5. Submit a pull request.

<p align="center">
  <img src="https://i.imgur.com/5nbPY1g.png" alt="Imagem logo" style="height: 100px;"/>
</p>

