#!/usr/bin/env python
# -*- coding: utf-8 -*-
import numpy as np
from scipy.special import comb, perm
import matplotlib.pyplot as plt

plt.rcParams['font.sans-serif'] = ['SimHei']
# plt.rcParams['font.sans-serif'] = ['STXIHEI']
plt.rcParams['axes.unicode_minus'] = False


class Handwriting:
    def __init__(self, line):
        self.line = line
        self.index_02 = None    # 保存拖动的这个点的索引
        self.press = None       # 状态标识，1为按下，None为没按下
        self.pick = None        # 状态标识，1为选中点并按下,None为没选中
        self.motion = None      # 状态标识，1为进入拖动,None为不拖动
        self.xs = list()        # 保存点的x坐标
        self.ys = list()        # 保存点的y坐标
        self.cidpress = line.figure.canvas.mpl_connect('button_press_event', self.on_press)  # 鼠标按下事件
        self.cidrelease = line.figure.canvas.mpl_connect('button_release_event', self.on_release)  # 鼠标放开事件
        self.cidmotion = line.figure.canvas.mpl_connect('motion_notify_event', self.on_motion)  # 鼠标拖动事件
        self.cidpick = line.figure.canvas.mpl_connect('pick_event', self.on_picker)  # 鼠标选中事件

        self.ctl_point_1 = None

    def on_press(self, event):  # 鼠标按下调用
        if event.inaxes != self.line.axes: return
        self.press = 1

    def on_motion(self, event):  # 鼠标拖动调用
        if event.inaxes != self.line.axes: return
        if self.press is None: return
        if self.pick is None: return
        if self.motion is None:  # 整个if获取鼠标选中的点是哪个点
            self.motion = 1
            x = self.xs
            xdata = event.xdata
            ydata = event.ydata
            index_01 = 0
            for i in x:
                if abs(i - xdata) < 0.02:  # 0.02 为点的半径
                    if abs(self.ys[index_01] - ydata) < 0.02: break
                index_01 = index_01 + 1
            self.index_02 = index_01
        if self.index_02 is None: return
        self.xs[self.index_02] = event.xdata  # 鼠标的坐标覆盖选中的点的坐标
        self.ys[self.index_02] = event.ydata
        self.draw_01()

    def on_release(self, event):  # 鼠标按下调用
        if event.inaxes != self.line.axes: return
        if self.pick is None:  # 如果不是选中点，那就添加点
            self.xs.append(event.xdata)
            self.ys.append(event.ydata)
        if self.pick == 1 and self.motion != 1:  # 如果是选中点，但不是拖动点，那就降阶
            x = self.xs
            xdata = event.xdata
            ydata = event.ydata
            index_01 = 0
            for i in x:
                if abs(i - xdata) < 0.02:
                    if abs(self.ys[index_01] - ydata) < 0.02: break
                index_01 = index_01 + 1
            self.xs.pop(index_01)
            self.ys.pop(index_01)
        self.draw_01()
        self.pick = None  # 所有状态恢复，鼠标按下到稀放为一个周期
        self.motion = None
        self.press = None
        self.index_02 = None

    def on_picker(self, event):  # 选中调用
        self.pick = 1

    def draw_01(self):  # 绘图
        self.line.clear()  # 不清除的话会保留原有的图
        self.line.set_title('Bezier曲线拟合手写笔迹')
        self.line.axis([0, 1, 0, 1])  # x和y范围0到1
        # self.bezier(self.xs, self.ys)  # Bezier曲线
        self.all_curve(self.xs, self.ys)
        self.line.scatter(self.xs, self.ys, color='b', s=20, marker="o", picker=5)  # 画点
        self.line.plot(self.xs, self.ys, color='black', lw=0.5)  # 画线
        self.line.figure.canvas.draw()  # 重构子图

    # def list_minus(self, a, b):
    #     list(map(lambda x, y: x - y, middle, begin))


    def controls(self, k, begin, middle, end):
        # if k > 0.5 or k <= 0:
        #     print('value k not invalid, return!')
        #     return

        diff1 = middle - begin
        diff2 = end - middle

        l1 = (diff1[0] ** 2 + diff1[1] ** 2) ** (1 / 2)
        l2 = (diff2[0] ** 2 + diff2[1] ** 2) ** (1 / 2)

        first = middle - (k * diff1)
        second = middle + (k * diff2)

        c = first + (second - first) * (l1 / (l2 + l1))

        # self.line.text(begin[0] - 0.2, begin[1] + 1.5, 'A', fontsize=12, verticalalignment="top",
        #                horizontalalignment="left")
        # self.line.text(middle[0] - 0.2, middle[1] + 1.5, 'B', fontsize=12, verticalalignment="top",
        #                horizontalalignment="left")
        # self.line.text(end[0] + 0.2, end[1] + 1.5, 'C', fontsize=12, verticalalignment="top",
        #                horizontalalignment="left")
        # xytext = [(first[0] + second[0]) / 2, min(first[1], second[1]) - 10]
        #
        arrow_props = dict(arrowstyle="<-", connectionstyle="arc3")
        # self.line.annotate('', first, xytext=xytext, arrowprops=dict(arrowstyle="<-", connectionstyle="arc3,rad=-.1"))
        # self.line.annotate('', c, xytext=xytext, arrowprops=arrow_props)
        # self.line.annotate('', second, xytext=xytext, arrowprops=dict(arrowstyle="<-", connectionstyle="arc3,rad=.1"))

        # label = '从左到右3个点依次分别为b\', c\', t,\n' \
        #         '满足条件 k = |b\'B| / |AB|, k = |c\'B| / |CB|\n' \
        #         '然后把线段(b\'c\')按 t 到 B的路径移动,\n' \
        #         '最后得到的两个端点就是我们要求的以B为顶点的控制点'
        # self.line.text(xytext[0], xytext[1], label, verticalalignment="top", horizontalalignment="center")
        self.line.plot([first[0], c[0], second[0]], [first[1], c[1], second[1]], linestyle='dashed', color='violet', marker='o', lw=0.3)

        first_control = first + middle - c
        second_control = second + middle - c

        # self.line.text(first_control[0] - 0.2, first_control[1] + 1.5, '控制点B\'', fontsize=9, verticalalignment="top",
        #                horizontalalignment="left")
        # self.line.text(second_control[0] + 0.2, second_control[1] + 1.5, '控制点B\'\'', fontsize=9,
        #                verticalalignment="top", horizontalalignment="left")
        x_s = [first_control[0], second_control[0]]
        y_s = [first_control[1], second_control[1]]

        # self.line.annotate('', xy=middle, xytext=c, arrowprops=dict(facecolor='b' headlength=10, headwidth=25, width=20))
        arrow_props['facecolor'] = 'blue'
        # arrow_props['headlength'] = 5
        # arrow_props['headwidth'] = 10
        # arrow_props['width'] = 5
        # self.line.annotate('', xy=c, xytext=middle, arrowprops=arrow_props)
        # self.line.annotate('', xy=first, xytext=first_control, arrowprops=arrow_props)
        # self.line.annotate('', xy=second, xytext=second_control, arrowprops=arrow_props)
        # self.line.plot([begin[0], middle[0], end[0]], [begin[1], middle[1], end[1]], lw=1.0, marker='o')
        self.line.plot(x_s, y_s, marker='o', lw=1, color='r', linestyle='dashed')
        # self.line.plot(x_s, y_s, lw=1.0)

        return first_control, second_control

    def all_curve(self, xs, ys):
        self.ctl_point_1 = None
        le = len(xs)
        if le < 3: return

        begin = [xs[0], ys[0]]
        middle = [xs[1], ys[1]]
        end = [xs[2], ys[2]]
        self.one_curve(begin, middle, end)

        for i in range(3, le):
            begin = middle
            middle = end
            end = [xs[i], ys[i]]
            self.one_curve(begin, middle, end)

        end = [xs[le - 1], ys[le - 1]]
        x = [middle[0], self.ctl_point_1[0], end[0]]
        y = [middle[1], self.ctl_point_1[1], end[1]]
        self.bezier(x, y)


    def one_curve(self, begin, middle, end):
        ctl_point1 = self.ctl_point_1

        begin = np.array(begin)
        middle = np.array(middle)
        end = np.array(end)

        ctl_point2, self.ctl_point_1 = self.controls(0.3, np.array(begin), np.array(middle), np.array(end))
        if ctl_point1 is None: ctl_point1 = begin

        xs = [begin[0], ctl_point1[0], ctl_point2[0], middle[0]]
        ys = [begin[1], ctl_point1[1], ctl_point2[1], middle[1]]
        self.bezier(xs, ys)

        # xs = [middle[0], self.ctl_point_1[0], end[0], end[0]]
        # ys = [middle[1], self.ctl_point_1[1], end[1], end[1]]
        # self.bezier(xs, ys)

    def bezier(self, *args):  # Bezier曲线公式转换，获取x和y
        t = np.linspace(0, 1)  # t 范围0到1
        le = len(args[0]) - 1

        self.line.plot(args[0], args[1], marker='o', color='r', lw=0.8)
        le_1 = 0
        b_x, b_y = 0, 0
        for x in args[0]:
            b_x = b_x + x * (t ** le_1) * ((1 - t) ** le) * comb(len(args[0]) - 1, le_1)  # comb 组合，perm 排列
            le = le - 1
            le_1 = le_1 + 1

        le = len(args[0]) - 1
        le_1 = 0
        for y in args[1]:
            b_y = b_y + y * (t ** le_1) * ((1 - t) ** le) * comb(len(args[0]) - 1, le_1)
            le = le - 1
            le_1 = le_1 + 1

        color = "yellowgreen"
        if len(args) > 2 : color = args[2]
        self.line.plot(b_x, b_y, color=color, linewidth='3')


fig = plt.figure(2, figsize=(12, 6))
ax = fig.add_subplot(111)  # 一行一列第一个子图
ax.set_title('手写笔迹贝赛尔曲线, 计算控制点图解')

handwriting = Handwriting(ax)
plt.xlabel('X')
plt.ylabel('Y')

# begin = np.array([20, 6])
# middle = np.array([30, 40])
# end = np.array([35, 4])
# handwriting.one_curve(begin, middle, end)
# myBezier.controls(0.2, begin, middle, end)
plt.show()
