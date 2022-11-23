from manim import *

def Node(text):
    result = VGroup()
    result.add(Rectangle(height=1/1.5, width=len(text)/2, color=BLACK))
    result.add(VGroup())
    for c in text:
        result[1].add(Text(c, fill_color=BLACK, font="Consolas", font_size=36))
    result[1].arrange_in_grid(cols=len(text), row_alignments='d'*len(text))
    return result

def Edge(s, e):
    return Arrow(start=s.get_center()+0.4*DOWN, end=e.get_center()+0.4*UP,
        buff=0.1, color=BLACK, stroke_width=3, max_tip_length_to_length_ratio=0,
        max_stroke_width_to_length_ratio=10)

def Line(s, e):
    return Arrow(start=s, end=e,
        buff=1, color=BLACK, stroke_width=3, max_tip_length_to_length_ratio=0,
        max_stroke_width_to_length_ratio=10)

def Crossbar(literals):
    height = len(literals)
    width = len(literals[0])
    result = VGroup()
    for y in range(height):
        for x in range(width):
            circle = Circle(color=BLACK, radius=0.35)
            literal = Text(literals[y][x], fill_color=BLACK, font_size=36, font="Consolas")
            result.add(VGroup(circle, literal))
    result.arrange_in_grid(rows=height, cols=width, buff=0.5)
    for y in range(height-1):
        for x in range(width):
            result.add(Line(result[y*width+x], result[(y+1)*width+x]))
    for y in range(height):
        for x in range(width-1):
            result.add(Line(result[y*width+x], result[y*width+x+1]))
    return result

class Diagram(Scene):
    def construct(self):
        self.camera.background_color = WHITE

        ade = Node("ade").shift(1.5*UP+2*LEFT)
        bc = Node("bc").shift(0.75*LEFT+2*LEFT)
        f = Node("f").shift(0.75*RIGHT+2*LEFT)
        one = Node("1").shift(1.5*DOWN+2*LEFT)
        self.add(ade)
        self.add(bc)
        self.add(f)
        self.add(one)
        self.add(Edge(ade[1][0], bc[1][0]))
        self.add(Edge(bc[1][0], one[1][0]))
        self.add(Edge(bc[1][1], one[1][0]))
        self.add(Edge(ade[1][1], f[1][0]))
        self.add(Edge(ade[1][2], f[1][0]))
        self.add(Edge(f[1][0], one[1][0]))

        self.add(Crossbar([["b", "1", "f", "0"],["0", "0", "1", "1"],["c", "1", "0", "0"],["a", "0", "d", "e"]]).shift(2*RIGHT))