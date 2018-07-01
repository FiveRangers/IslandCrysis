# FBI WARNINGS
---
## cloth类还是很不稳的，只能用于展示不能用的，最好就不要改main.cpp里面跟织物仿真有关的代码
---
# 说明
## 1. 使用
main.cpp里面跟织物仿真有关的代码就是下面这两部分，最好就不要改了（改改颜色应该是没问题，其他的就保持原样吧）
```cpp
  // 初始化一块15*15的布料
	cloth flag(15, 15);
	// 获取布料的顶点数组指针
	float * flagVertices = new float[flag.getRow()*flag.getCol() * 3];
	flag.getVerticeArray(flagVertices);

	// flag VAO and VBO
	unsigned int flagVAO, flagVBO;
	glGenVertexArrays(1, &flagVAO);
	glGenBuffers(1, &flagVBO);
	glBindVertexArray(flagVAO);
	glBindBuffer(GL_ARRAY_BUFFER, flagVBO);
	glBufferData(GL_ARRAY_BUFFER, flag.getRow()*flag.getCol() * 3 * sizeof(float), flagVertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 获取布料的索引数组指针
	shared_ptr<unsigned int> indexPtr = flag.getIndexArray();
	unsigned int *index = new unsigned int[(flag.getRow() - 1)*(flag.getCol() - 1) * 2 * 3];
	flag.getIndexArray(index);

	// flag EBO
	unsigned int flagEBO;
	glGenBuffers(1, &flagEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flagEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, flag.getRow()*flag.getCol() * 6 * sizeof(unsigned int), index, GL_STATIC_DRAW);

	Shader flagShader("FlagVS.glsl", "FlagFS.glsl");
```
```cpp
    flagShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(8.0f, 10.0f, 13.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		flagShader.setMat4("model", model);
		flagShader.setMat4("view", view);
		flagShader.setMat4("projection", projection);
		flagShader.setVec4("color", glm::vec4(1.0f, 0.0f,0.0f, 0.0f));
		glBindVertexArray(flagVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flagEBO);
		glDrawElements(GL_TRIANGLES, (flag.getRow()- 1)*(flag.getCol() - 1) * 6, GL_UNSIGNED_INT, 0);
		flag.Update(flagVertices);

		// 更新缓存中的织物顶顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, flagVBO);
		glBufferData(GL_ARRAY_BUFFER, flag.getRow()*flag.getCol() * 3 * sizeof(float), flagVertices, GL_DYNAMIC_DRAW);
```
## 2. cloth类
### 2.1 设计
- 每个cloth类对象都是一个 n*n的网格。为了方便调用glm库函数来进行各种向量运算，所以储存顶点是用了一个矩阵来进行储存的，矩阵中的每个元素都是一个glm::vec3，表示了网格中每个点的位置。
- 初始化时是按照这样的第i行第j列的质点的坐标为(i,j)来进行初始化的，相邻质点的距离为1个单位
- 绘制这个网格n*n是利用索引数组来绘制的，整个网格由（n-1）*（n-1）个小正方形组成，沿着从左上到右下的对角线划分每个小正方形又分成两个三角形组成。根据这个规律生成顶点索引数组来进行绘制
- 因为顶点数据都存在一个矩阵中且矩阵元素都是向量，所以可以通过getVerticesArray函数来获取一个跟平常储存了顶点数据的数组，然后用这个数组来进行绘制（代码中是用了两个vector来实现矩阵的，不知道能不能直接返回数据的首地址来绘制，因为感觉glm::vec3好像并不是按(x,y,z)这样的顺序存在内存里的）
### 2.2 相关问题说明
- cloth类里面有些函数其实是要舍弃的，但是我还没有删掉，所以有些函数是有点重复的
- 因为写之前没有看过explosion类、particle类这些类的实现，所以类接口不大一致。原本Simulation函数就是实现更新顶点坐标的，但是为了跟其他的类代码保持一致所以就在外面在包了一层Update函数
- Update函数里面这一段原来是用在我自己的项目来实现视口变换的，功能跟乘上view矩阵实现视口变换是一样的，按理说在FirstGLFW里面删掉这一段也应该可以的，但是在里面删掉了这一段就不行，我也不知道为什么。。。
  ```cpp
	for (int i = 0; i < row*col * 3 && row>1; i++)
	{
		vertices[i] = vertices[i] / (row - 1);
		if (vertices[i] > 1) vertices[i] = 1;
		else if (vertices[i] < -1) vertices[i] = -1;
	}
  ```
- 在项目里面实现的织物仿真是没有加重力的，只有弹簧力、缓冲力和风力，因为加了重力之后对K、L的初值设定就有点麻烦，做出来的效果也不行。
- 实际上实现的红旗飘的效果是不能从侧面看的，从侧面看是会发现旗子好像没有在动（感觉就是z坐标没有变），但是从正面看还比较像在迎风飘。
