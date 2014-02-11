8a9
> 
16a18
> 
19c21
<          If_Else statement:
---
>          If_Else statement:	
25c27
<          Condition False : Goto (BB 5)
---
>             Condition False : Goto (BB 5)
34d35
< 
40a42
> 
43c45
<          If_Else statement:
---
>          If_Else statement:	
49c51
<          Condition False : Goto (BB 7)
---
>             Condition False : Goto (BB 7)
1,38d0
< Evaluating Program
<    Global Variables (before evaluating):
<          a : 0
<    Evaluating Procedure main
<       Local Variables (before evaluating):
<          b : undefined
< 
< 
<       Basic Block: 2
< 
<          Asgn:
<             LHS (Name : b)
<             RHS (Num : 4)
<          b : 4
< 
< 
<          Asgn:
<             LHS (Name : a)
<             RHS (Num : 3)
<          a : 3
< 
< 
<          Asgn:
<             LHS (Name : a)
<             RHS (
<             Condition: GT
<                LHS (Name : a)
<                RHS (Name : b))
<          a : 0
< 
< 
<          Return <NOTHING>
< 
< 
<       Local Variables (after evaluating):
<          b : 4
<    Global Variables (after evaluating):
<          a : 0
10a11
> 
18d18
< 
24,25c24
< 
<          If_Else statement:
---
>          If_Else statement:	
31c30
<          Condition True : Goto (BB 3)
---
>             Condition True : Goto (BB 3)
41,43d39
<          Goto statement:
<             Successor: 5
<          GOTO (BB 5)
51d46
< 
1,79d0
< Evaluating Program
<    Global Variables (before evaluating):
<    Evaluating Procedure main
<       Local Variables (before evaluating):
<          a : undefined
<          b : undefined
<          c : undefined
<          d : undefined
<          e : undefined
<          f : undefined
< 
< 
<       Basic Block: 2
< 
<          Asgn:
<             LHS (Name : a)
<             RHS (Num : 3)
<          a : 3
< 
< 
<          Asgn:
<             LHS (Name : b)
<             RHS (Num : 4)
<          b : 4
< 
< 
<          Asgn:
<             LHS (Name : c)
<             RHS (Num : 5)
<          c : 5
< 
< 
<          Asgn:
<             LHS (Name : d)
<             RHS (Num : 6)
<          d : 6
< 
< 
<          Asgn:
<             LHS (Name : e)
<             RHS (Num : 7)
<          e : 7
< 
< 
<          Asgn:
<             LHS (Name : f)
<             RHS (Num : 8)
<          f : 8
< 
< 
<          Asgn:
<             LHS (Name : a)
<             RHS (
<             Condition: NE
<                LHS (
<             Condition: EQ
<                LHS (
<             Condition: LE
<                LHS (
<             Condition: LE
<                LHS (Name : c)
<                RHS (Num : 7))
<                RHS (Name : e))
<                RHS (Name : d))
<                RHS (Name : f))
<          a : 1
< 
< 
<          Return <NOTHING>
< 
< 
<       Local Variables (after evaluating):
<          a : 1
<          b : 4
<          c : 5
<          d : 6
<          e : 7
<          f : 8
<    Global Variables (after evaluating):
1,65d0
< Evaluating Program
<    Global Variables (before evaluating):
<    Evaluating Procedure main
<       Local Variables (before evaluating):
<          a : undefined
<          b : undefined
<          c : undefined
< 
< 
<       Basic Block: 2
< 
<          Asgn:
<             LHS (Name : a)
<             RHS (Num : 3)
<          a : 3
< 
< 
<          Asgn:
<             LHS (Name : b)
<             RHS (Num : 4)
<          b : 4
< 
< 
<          Asgn:
<             LHS (Name : c)
<             RHS (Num : 2)
<          c : 2
< 
< 
<          If_Else statement:
<             Condition: GT
<                LHS (Name : a)
<                RHS (Num : 2)
<             True Successor: 3
<             False Successor: 4
<          Condition True : Goto (BB 3)
< 
<       Basic Block: 3
< 
<          Asgn:
<             LHS (Name : c)
<             RHS (
<             Condition: GT
<                LHS (
<             Condition: GT
<                LHS (Name : a)
<                RHS (Name : b))
<                RHS (Name : c))
<          c : 0
< 
< 
<          Goto statement:
<             Successor: 5
<          GOTO (BB 5)
< 
<       Basic Block: 5
< 
<          Return <NOTHING>
< 
< 
<       Local Variables (after evaluating):
<          a : 3
<          b : 4
<          c : 0
<    Global Variables (after evaluating):
