# Contributing to SuperNOVAS


The _SuperNOVAS_ library is for everyone. While I, Attila Kovacs, am the maintainer of this library at the 
Smithsonian, I absolutely rely on developers like you to make it better. Whether there is a nagging issue you would 
like to fix, or a new feature you'd like to see, you can make a difference yourself. I very much encourage you to 
make this project a little bit your own, by submitting pull requests with fixes and enhancement. When you are ready, 
here are the typical steps for contributing to the project:

1. Old or new __Issue__? Whether you just found a bug, or you are missing a much needed feature, start by checking 
open (and closed) [Issues](https://github.com/Smithsonian/SuperNOVAS/issues). If an existing issue seems like a 
good match to yours, feel free to speak up, comment, or to offer help in resolving it. If you find no issues that 
match, go ahead and create a new one.

2. __Fork__. Is it something you'd like to help resolve? Great! You should start by creating your own fork of the 
repository so you can work freely on your solution. I recommend that you place your work on a branch of your fork, 
which is named either after the issue number, e.g. `issue-192`, or some other descriptive name, such as 
`fast-nutations`.

3. __Develop__. Experiment on your fork/branch freely. If you run into a dead-end, you can always abandon it (which is 
why branches are great) and start anew. You can run your own tests locally using `make test` before committing your 
changes. If the tests pass, you should also try running `make all` and to ensure that all components of the package 
and its API documentation are also in order. Remember to synchronize your `master` branch by fetching changes from 
upstream every once in a while, and merging them into your development branch. Don't forget to:

   - Add __doxygen__ markup your new code. You can keep it sweet and simple, but make sure it properly explains your 
   globally exposed functions, their arguments and return values. You should also cross-reference other functions / 
   constants that are similar, related, or relevant to what you just added.

   - Add __Unit Tests__. Make sure your new code has as close to full unit test coverage as possible. You can add
   tests to `test/src/test.c` as appropriate, and if you add new source files, you may need to adjust `test/Makefile`
   so that `gcov` coverage is generated for your new source files. 
   You should aim for 100% diff coverage. When pushing changes to your fork, you can get a coverage report by checking 
   the Github Actions result of your commit (click the Coveralls link), and you can analyze what line(s) of code need 
   to have tests added. Try to create tests that are simple but meaningful (i.e. check for valid results, rather than 
   just confirm existing behavior), and try to cover as many realistic scenarios as appropriate. Write lots of tests 
   if you need to.

4. __Pull Request__. Once you feel your work can be integrated, create a pull request from your fork/branch. You can 
do that easily from the github page of your fork/branch directly. In the pull request, provide a concise description 
of what you added or changed. Your pull request will be reviewed. You may get some feedback at this point, and maybe 
there will be discussions about possible improvements or regressions etc. It's a good thing too, and your changes will 
likely end up with added polish as a result. You can be all the more proud of it in the end!

5. If all goes well, your pull-request will get merged, and will be included in the upcoming release of 
_SuperNOVAS_. Congratulations for your excellent work, and many thanks for dedicating some of your time for making 
this library a little bit better. There will be many who will appreciate it. :-)


If at any point you have questions, or need feedback, don't be afraid to ask. You can put your questions into the 
issue you found or created, or your pull-request, or as a Q&amp;A in 
[Discussions](https://github.com/Smithsonian/SuperNOVAS/discussions).


