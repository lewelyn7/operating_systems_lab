l1 = ['a.txt','b.txt','biblia.txt','bible.txt','dlugi1.txt','dlugi2.txt','lalka.txt','3muszkiet.txt']
l2 = l1
for i in l1:
    for j in l1:
        if(i != j):
            print(i+":"+j, end=' ')
