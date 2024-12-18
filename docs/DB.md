
# Lignum Database


Database consists of two tables : 

 `persons` - uniqely identified individuals;

 `couples` - uniqely identified pairs of individuals, referred by id.


Database could be represented as a directed graph of special form. Such graph has `individual nodes`,
representing perons, and `family nodes` representing couples. Edge, that goes FROM individual
TO family node represents partnership. Edge, going FROM family node TO individual node represents
parents-child relationship.

 The database should meet the following requirements

 - Graph must be connected

 - Graph is nonempty (???)

 - Graph must be a tree : no cycles are allowed. 

 - In couple person1_id is always nonzero.




## Removing element

In order node to be removed properly, it should basically be a leaf.


