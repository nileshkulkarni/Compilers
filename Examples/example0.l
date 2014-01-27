

%filenames="Scanner"
%lex-source="Scanner.cc"

%%

a*b                        {std::cout << "Token 1 found\n";}
c+                         {std::cout << "Token 2 found\n";}

