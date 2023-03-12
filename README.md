# JNP1-Projects
Projects created during "anguages and tools for programming I" (Języki i narzędzia programowania 1) during the academic year 2022/23 at University of Warsaw.

Throughout this course, we have learned programming paradigms in C++ (OOP, functional, imperative).

The projects were made in groups of two or three, time to make each one was approximately 2 weeks.

This README is inspired by [@wojsza05](https://github.com/wojsza05).

My scores:

| TASK           | CO-AUTHORS                                   | TESTS          | CODE REVIEW    |
| :------------: | :------------------------------------------: | :------------: | :------------: |
| **_top7_**     | [@wojsza05](https://github.com/wojsza05) | **6** / 6 _pt_ | **4** / 4 _pt_ |
| **_hash_**     | [@SzymonM2137](https://github.com/SzymonM2137) | **5** / 6 _pt_ | **3** / 4 _pt_ |
| **_moneybag_** | [@KDadej](https://github.com/KDadej) | **4** / 6 _pt_ | **4** / 4 _pt_ |
| **_organism_** | [@JKuziola](https://github.com/JKuziola) [@SzymonSzmigielski](https://github.com/SzymonSzmigielski) | **6** / 6 _pt_ | **4** / 4 _pt_ |
| **_kvfifo_**   | [@zortenburger](https://github.com/zortenburger)    | **3** / 6 _pt_ | **4** / 4 _pt_ |
| **_worldcup_** | [@BrutBurger](https://github.com/BrutBurger)  | **4** / 6 _pt_ | **3** / 4 _pt_ |
| **_images_**   | -                                            | **6** / 6 _pt_ | **4** / 4 _pt_ |

Here is the small description of each task.

## top7
The main goal of the task was to make rank list of music hits and handling the voting procedures.

_Topics_: STL, regex.

## hash

The task was to write an own implementation of a hash table, which uses the provided hash function.

_Topics_: More STL, merging C++ code with C code, initializing global objects (preventing 'static initialization order fiasco').

## moneybag

The task was to write representation of 3 types of money.
The main goal was to learn OOP, but also handling exceptions and making own operators for own classes.

_Topics_: Object-oriented programming in C++, classes, constructors, operators 
overloading, exceptions, initialization lists, keywords: const, constexpr, etc.

## organism

The main goal was to implement interactions between organisms, which were templates; interactions varied based on what type of organism meet. 

_Topics_: Templates, classes, constexpr functions, templates with variable 
number of arguments, fold expressions.

## kvfifo

The task was to write an implementation of FIFO queue, where each element had a key an a value. Moreover, it was required to write different operations, 
such as moving all elements with the same key to the start/end, giving maximum/minimum value of a specific key and so on.

_Topics_: Copy-on-write, move semantics, smart pointers, exceptions, STL, 
templates.


## worldcup

The task was to write a simulation of a board game; in this case it was a football world cup. The main goal was to use smart pointers to keep players and
fields on the board in a smart way.

_Topics_: Object-oriented programming in C++, making well-thought classes, inheritance, 
exceptions, smart pointers.

## images

The task was to write functions which create images, for example: strip, checker, circle, rotating objects, scaling, adding color features.

The main goal was to provide implement composition and lift of a functions (which are inside 'functional.h') and functions which create specific
(which are inside 'images.h'). Other files were provided by the authors.

_Topics_: Functional programming in C++, lambda expressions, templates, fold 
expressions.
