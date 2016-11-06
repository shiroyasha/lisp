# Lisp

Following the tutorial from [Build Your Own Lisp](http://www.buildyourownlisp.com/).

## Building from source

Install dependencies:

``` bash
sudo apt-get install libedit-dev
```

Clone the source code:

``` bash
git clone https://github.com/shiroyasha/lispy ~/lispy
```

Build binary:

``` bash
cd ~/lispy
make
```

Run lispy:

``` bash
cd ~/lispy
./lispy
```

## Examples

Arithmetic:

``` lisp
> + 1 2
3

> * 1 2
2

> * (+ 1 2) (- 10 8)
6
```

Lists:

``` lisp
> { 1 2 3 }
{1 2 3}

> head {1 2 3}
1

> tail {1 2 3}
{2 3}

> join {1 2 3} {5 6 7}
{1 2 3 5 6 7}

> eval {+ 1 2}
3
```

Variables:

``` lisp
> def {a b c d} 1 90 20 87
()

> a
1

> (+ b c)
110
```

Functions:

``` lisp
> fun {add-together a b} {+ a b}
()

> add-together 32 18
50
```

Macros (fun fact: 'fun' is actually a macro defined in the language):

``` lisp
> def {fun} (\ {args body} {def (list (head args)) (\ (tail args) body)})
()

> fun {my-function a b c} {* a b c}
()

> my-function 1 2 3
6
```
