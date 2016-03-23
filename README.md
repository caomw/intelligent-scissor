## Intelligent scissor ##

Author : QIN Tong

Function : Cut the image like photoshop, i.e.find some certain coutour on the image, and get the image with coutour and mask.
		   
Code: C++ in QT Creator

###Implementation###

Data structures:	
```c++
struct Node{
	double linkCost[8];
	int state;
	double totalCost;
	Node *prevNode;
	int column, row; 
}
```

Dijkstra's algorithm:
```
Begin:

    initialize the priority queue pq to be empty;

    initialize each node to the INITIAL state;

    set the total cost of seed to be zero and make seed the root of the minimum path tree ( pointing to NULL ) ;

    insert seed into pq;

    while pq is not empty 

        extract the node q with the minimum total cost in pq;

        mark q as EXPANDED;

        for each neighbor node r of q  

            if  r has not been EXPANDED

                if  r is still INITIAL

                    make q be the predecessor of r ( for the the minimum path tree );

                    set the total cost of r to be the sum of the total cost of q and link cost from q to r as its total cost;

                    insert r in pq and mark it as ACTIVE;

                else if  r is ACTIVE, e.g., in already in the pq 

                    if the sum of the total cost of q and link cost between q and r is less than the total cost of r

                        update q to be the predecessor of r ( for the minimum path tree );

                        update the total cost of r in pq;

	End
```

###Usage###
1. Load one image;

	![image](https://github.com/qintony/intelligent-scissor/blob/master/picture/main_ui.png)

2. Click start;

3. Click one point as fisrt seed;

4. Move mouse, get the coutour as you want;

	![image](https://github.com/qintony/intelligent-scissor/blob/master/picture/get_contour.png)

5. Save image or mask.

	![image](https://github.com/qintony/intelligent-scissor/blob/master/picture/get_mask.png)




###Other things###
In the debug mode, you can see the cost graph, min cost map and other useful debug infomation.

![image](https://github.com/qintony/intelligent-scissor/blob/master/picture/cost_graph.png)
	
It's my first cv project in COMP 5212 HKUST. Thanks for prof CK Tang! 

Any problem contact with me! qintonguav@gmail.com
