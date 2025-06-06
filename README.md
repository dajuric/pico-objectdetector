# Let's Create An Ultra Fast Object Detector

An improved and faster variant of a popular [Viola and Jones](https://www.cs.cmu.edu/~efros/courses/LBMV07/Papers/viola-cvpr-01.pdf) detection algorithm is implemented in JavaScript according to [this paper](https://arxiv.org/pdf/1305.4537.pdf).   
*Training is implemmeted in C++ an the code is available in an associated repository.*   

![Samples](docs/samples.gif)

This implementation introduces several enhancements compared to the classic Viola-Jones approach:
1. Binary pixel comparisons replace Haar features — no need for integral images.   
2. Full decision trees are used instead of simple decision stumps.   
3. GentleBoost replaces AdaBoost for improved training stability and accuracy.   

## 1) Run It in Your Browser
The included demo runs entirely in JavaScript — under 120 lines of code — and achieves over 100 FPS, even on a smartphone.
Don’t just take our word for it - in fact, test it for yourself.

## 2) Step-by-Step Tutorial
A full, beginner-friendly tutorial is included, with step-by-step explanations.
Accompanying articles are available on my personal website, guiding you through everything from the basics to advanced implementation details.
