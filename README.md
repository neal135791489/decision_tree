# 建立decision tree, 使用c

Note:
- 1.範例屬性id 0->age, 1->income, 2->student, 3->credit_rating
- 2.產生子節點演算法概述
  - a.產生root
  - b.將節點及表格帶入函式
  - c.若皆為相同class則returnd.若只剩1行, 則取多數當class, returne
  - d.分別計算大表格的info及各屬性的infoDa, 找出最大的gain, 並標注節點為由此屬性分支
  - e.切割最多3個表格, 及各別產生子節點, 回到b.
