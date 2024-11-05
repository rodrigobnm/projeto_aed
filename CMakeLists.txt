cmake_minimum_required(VERSION 3.10)
project(MeuJogoSDL C)

# Define a versão do padrão C
set(CMAKE_C_STANDARD 11)

# Adicione a SDL2 usando o módulo FetchContent
include(FetchContent)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.26.0  # Coloque a versão desejada
)
FetchContent_MakeAvailable(SDL2)

# Adicione o executável e vincule a biblioteca SDL2
add_executable(meu_jogo src/main.c)
target_link_libraries(meu_jogo SDL2::SDL2 SDL2::SDL2main)
