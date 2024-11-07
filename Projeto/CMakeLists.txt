cmake_minimum_required(VERSION 3.10)
project(JogoDeCartas)

# Definir o padrão C
set(CMAKE_C_STANDARD 99)

# Localização dos arquivos fonte
set(SOURCE_FILES src/main.c)

# Adicionar executável
add_executable(JogoDeCartas ${SOURCE_FILES})

# Incluir diretórios de cabeçalhoscm
include_directories(src)

# Encontrar e linkar bibliotecas SDL2, SDL2_image e SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(JogoDeCartas SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)
