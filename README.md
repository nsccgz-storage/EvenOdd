# evenodd

## 赛题简介

存储系统保证容错的方式有两种，一种是多副本，即一份数据存储多份；另外一种方式是纠删码。纠删码和多副本相比，可以以更少的存储开销保证容错，因此，可以大大降低存储成本。在保证存储可靠性的同时，怎样在有限的资源下获得最好的编码，解码，和修复性能对存储系统来说十分重要。本次初赛赛题关注一个仅由异或操作便可以完成编解码的、最多可容两错的纠删码——EVENODD。本次赛题的目标是，在保证编解码正确性的基础上，在有限的系统资源下优化 EVENODD 各项操作的性能。具体的赛题说明请参照附件。


## 大文件的 evenodd

- 1. 把文件切分成不同的小块，不同的小块进行 encoding 和 decoding
- 2. 不切分文件，但是目前的 encoding 和 decoding 算法需要修改