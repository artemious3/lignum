
# Lignum Database


Database consists of two tables : 

 `persons` - uniqely identified individuals;

 `couples` - uniqely identified pairs of individuals.


Database could be represented as a directed graph of special form. Such graph has `individual nodes`,
representing perons, and `family nodes` representing couples. Edge, that goes FROM individual
TO family node represents partnership. Edge, going FROM family node TO individual node represents
parents-child relationship.

 The database should meet the following requirements

 - Graph must be connected

 - Graph is nonempty 

 - Graph must be a tree : no cycles are allowed [^1] 

 - For any couple person1_id =/= 0.

 - If person is in couple (person1_id, 0), this couple is the ONLY they consist.[^2]

 - Couple (person1_id, 0) is REQUIRED to have children. If some operation resulted in opposite, 
 couple must be removed.



[^1]: Incests can be handled by duplicating one of relatives with special mark (TBD)

[^2] : It's unclear how multiple _unkown_ partners or even one unkown and
multile known partners should be rendered.
