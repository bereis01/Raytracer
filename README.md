# **Ray Tracing: Geração de Imagens**

**DCC027** Computação Gráfica

Universidade Federal de Minas Gerais **2025H2**

**Bernardo Reis de Almeida**

---

A documentação completa deste trabalho está no arquivo `docs.pdf`.

## Instruções para Compilação

Na raiz do projeto, execute os comandos:

```sh
$ cmake -S . -B build
$ cmake --build build
```

## Instruções de Execução

Uma vez compilada, para executar a ferramenta, utilize o comando:

```sh
$ ./build/raytracer input_file output_file [width height num_rays max_recursion defocus_angle focus_distance]
```

Os parâmetros entre colchetes são opcionais. Lembrando que, para um parâmetro opcional ser passado, os demais antes dele também devem ser passados.

## Execução das Renderizações de Exemplo

Para executar as renderizações dos arquivos de especificação enunciados, execute:

```sh
$ ./build/raytracer test1.in test1.ppm
$ ./build/raytracer test2.in test2.ppm
$ ./build/raytracer test3.in test3.ppm
$ ./build/raytracer test4.in test4.ppm
$ ./build/raytracer test5.in test5.ppm
$ ./build/raytracer test6.in test6.ppm
```

Para executar as renderizações "extras" apresentadas na documentação, execute:

```sh
$ ./build/scene1 dummy scene1.ppm
$ ./build/scene2 dummy scene2.ppm
$ ./build/scene3 dummy scene3.ppm
```

...estes últimos comandos utilizam executáveis pré-compilados disponibilizados junto com o projeto. Eles não aceitam os parâmetros extras.

Lembrando que a renderização pode demorar.