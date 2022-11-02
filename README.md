# Crossbar Synthesis and Verification Tool

### How To Run

Run the following commands in a terminal:
```
git clone https://github.com/ethanpartidas/CSVT.git
cd CSVT
make
./test
```
On Windows, the executable will be called test.exe

### The Problem with BDDs

There is an inherent limitation to generating crossbars from BDDs. Each node in a BDD has at most 2
edges coming from it, so the total number of edges is proportional to the number of nodes. On the
crossbar, the nodes represent rows and columns, and the edges represent input memristors. Thus, the
number of input memristors is O(n) whereas the number of intersections in the crossbar is O(n^2),
where n is the number of nodes in the BDD. For large n, this creates crossbars that are very
sparsely populated by input memristors. As seen in the example 25 variable crossbar, it is mostly
0s.

### An Alternative Approach

We now have an incentive to find an alternative representation of logic functions that can
be converted into more compact crossbars. Let's look to the structure of the crossbar to
see what such a representation might look like:

```
   C0  C1  C2
R0 d---e---f
   |   |   |
R1 a---b---c
```

The input is on the bottom row and can take 3 possible paths to the output via any of the
3 columns. Before entering any of those columns, it must satisfy the literal at the intersection
between them. The logical representation of this looks like:

```
a(C0->R0) + b(C1->R0) + c(C2->R0)
```
where x->y represents the logic function that connects x to y

For this crossbar, this gives us ad+be+cf, but we can make those inner logic functions more complex
by recursively applying the same principle. Each column can take many rows to get to the output, and
as such will have a similar format for its logic function: a sum of terms. Here is an example:

```
   C0  C1  C2  C3
R0 g---h---i---1
   |   |   |   |
R1 d---e---f---1
   |   |   |   |
R2 a---b---c---0
```

This looks a lot more complicated, but all I've done here add an extra row of variables, and connect
R1 to R0, so they both work as output rows. The logic representation of this crossbar is:

```
a(d + g) + b(e + h) + c(f + i)
```

We've converted this compact crossbar into a kind of SOP expression, with singular literals factored
out of the terms. As you might be able to guess, this factoring can be done recursively with the
inner logic functions. We're starting to approach a method of taking an SOP expression and
converting it to a format that can generate compact crossbars. Let's walk through an example:

```
acd + ab'd + de + bcf
```

To eliminate as many literals, and thus memristors, as possible, let's factor the most common
literal.

```
d(ac + ab' + e) + bcf
```

Recursively factor the inner expression.

```
d(a(c + b') + e) + bcf
```

To make the process clearer, I will also "factor" the term on the right.

```
d(a(c+b') + e) + b(c(f))
```

On the outermost layer, we have d and b as our 2 possible ways to get onto a column.

```
d---b
```

On each column, we implement the inner expression, laying out the variables in a column. Let's
start with the left term.

```
e---0
|   |
a---0
|   |
d---b
```

We've completed the path for e. Let's make that row the output row and not move it from now on.
Next, we lay out the inner expression for a in a row, dodging the existing columns.

```
e---0---1---1
|   |   |   |
a---0---c---b'
|   |   |   |
d---b---0---0
```

Those are also complete, so we've merged their columns with the output row. Let's now work on the
right term.

```
e---0---1---1
|   |   |   |
0---c---0---0
|   |   |   |
a---0---c---b'
|   |   |   |
d---b---0---0
```

You can see how the extra parentheses let us know when we switch between rows and columns. Also
notice how we kept the output row on top. Lastly, the f will complete the route to the output, so we
can connect it to any existing output row or column.

```
e---0---1---1
|   |   |   |
0---c---f---0
|   |   |   |
a---0---c---b'
|   |   |   |
d---b---0---0
```

And Tada! A very compact crossbar for this logic function. BDDs tend to generate a lot of postitive
and negative literal pairs, so I'm confident that they couldn't get this result.

### Further Improvements

There are still some limitations with this approach. Take this adversarial input:

```
a(g + h + i) + b + c + d
```

The algorithm above generates the following crossbar:

```
i---1---1---1
|   |   |   |
h---1---0---0
|   |   |   |
g---1---0---0
|   |   |   |
a---b---c---d
```

This has the same sparseness problem that the BDD method has. Thus, the new algorithm CAN generate
optimal crossbars, but is not guaranteed to. Whereas the BDD method is guaranteed to be suboptimal.

An improvement on the above crossbar is as follows:

```
0---1---1---1
|   |   |   |
1---g---h---i
|   |   |   |
a---b---c---d
```

This represents converting the input expression to:

```
a(1(g + h + i)) + b + c + d
```

This method of "folding" the long column onto an already-long row saves space. Another example:

```
0---1---1---1
|   |   |   |
1---j---k---l
|   |   |   |
1---g---h---i
|   |   |   |
a---b---c---d
```

```
a(1(g + h + i) + 1(j + k + l)) + b + c + d
```

This shows we can make several "folds" on the same column/row. The extra factoring of these 1s in
the input expression is likely best done after the initial generation of the crossbar. Some kind of
heuristic could be used to analyze the inefficient crossbar and determine where these folds could be
inserted to save the most space.

### General Boolean Expressions

What if the expression we are given is in POS form?

Simply negate it and add a NOT to the output of the crossbar. 

What if the expression is a mix of SOP and POS terms?

We would have to expand the terms out. However, we would end up implementing the same subexpression
several times. For example:

```
(a + b)X = aX + bX
```

Luckily, I've already implemented the reuse of subexpressions in the algorithm. For this example, we
would just need to make a and b's child pointers point to the same X node. Thus, we can write a new
function that takes any boolean expression, not just SOP expressions, and converts it into the
appropriate tree.

This also gives us the opportunity to use an external tool to factor the expression. For the new
function, I won't implement the greedy factoring and instead I'll just assume that the given
expression is optimally factored.
