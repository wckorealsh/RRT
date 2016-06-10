#include "GridRRTWidget.hpp"
#include <planning/Path.hpp>

using namespace RRT;
using namespace Eigen;


GridRRTWidget::GridRRTWidget() {
    _stateSpace = make_shared<GridStateSpace>(rect().width(),
                                        rect().height(),
                                        40,
                                        30);
    _biRRT = new BiRRT<Vector2f>(_stateSpace);

    _waypointCacheMaxSize = 15;

    //  setup birrt
    _biRRT->setStartState(Vector2f(50, 50));
    _biRRT->setGoalState(Vector2f(width() / 2.0, height() / 2.0));
    _biRRT->setStepSize(10);
    _biRRT->setGoalMaxDist(12);

    //  register for mouse events
    setMouseTracking(true);
    _draggingStart = false;
    _draggingGoal = false;
}

void GridRRTWidget::slot_reset() {
    //  store waypoint cache
    vector<Vector2f> waypoints;
    if (_biRRT->startSolutionNode() && _biRRT->goalSolutionNode()) {
        waypoints = _previousSolution;
        if (waypoints.size() > 0) {
            //  don't keep the start or end states
            waypoints.erase(waypoints.begin());
            waypoints.erase(waypoints.end());

            //  down-sample
            Planning::DownSampleVector<Vector2f>(waypoints, _waypointCacheMaxSize);
        }
    } else {
        _previousSolution.clear();
    }

    _biRRT->reset();

    _biRRT->setWaypoints(waypoints);

    emit signal_stepped(0);
    update();
}

void GridRRTWidget::slot_clearObstacles() {
    _stateSpace->obstacleGrid().clear();

    update();
}

void GridRRTWidget::slot_setGoalBias(int bias) {
    _biRRT->setGoalBias((float)bias / 100.0f);
}

void GridRRTWidget::slot_setWaypointBias(int bias) {
    _biRRT->setWaypointBias((float)bias / 100.0f);
}

void GridRRTWidget::slot_setStepSize(double step) {
    _biRRT->setStepSize(step);
}

bool GridRRTWidget::hasSolution() const {
    return _biRRT->startSolutionNode() != nullptr;
}

void GridRRTWidget::step(int numTimes) {
    for (int i = 0; i < numTimes; i++) {
        _biRRT->grow();
    }

    //  store solution
    _previousSolution.clear();
    if (_biRRT->startSolutionNode() != nullptr) {
        _biRRT->getPath(_previousSolution);
        Planning::SmoothPath<Vector2f>(_previousSolution, *_stateSpace);
    }

    emit signal_stepped(_biRRT->iterationCount());
    update();
}

QPointF GridRRTWidget::pointFromNode(const Node<Vector2f> *n) {
    return QPointF(n->state().x(), n->state().y());
}

void GridRRTWidget::paintEvent(QPaintEvent *p) {
    RRTWidget::paintEvent(p);

    QPainter painter(this);

    drawObstacles(painter, _stateSpace->obstacleGrid());

    //  draw previous solution
    if (_previousSolution.size() > 0) {
        painter.setPen(QPen(Qt::yellow, 3));
        Vector2f prev;
        bool first = true;
        for (const Vector2f &curr : _previousSolution) {
            if (first) {
                first = false;
            } else {
                painter.drawLine(QPointF(prev.x(), prev.y()), QPointF(curr.x(), curr.y()));
            }
            prev = curr;
        }
    }


    //  draw waypoint cache
    if (_biRRT->waypoints().size() > 0) {
        float r = 2;    //  radius to draw waypoint dots

        painter.setPen(QPen(Qt::lightGray, 3));
        for (const Vector2f &waypoint : _biRRT->waypoints()) {
            painter.drawEllipse(QPointF(waypoint.x(), waypoint.y()), r, r);
        }
    }

    //  draw trees
    drawTree(painter, _biRRT->startTree(), _biRRT->startSolutionNode());
    drawTree(painter, _biRRT->goalTree(), _biRRT->goalSolutionNode(), Qt::darkGreen);

    //  draw root as a red dot
    painter.setPen(QPen (Qt::red, 6));
    QPointF rootLoc = pointFromNode(_biRRT->startTree().rootNode());
    painter.drawEllipse(rootLoc, 2, 2);

    //  draw goal as a green dot
    QPointF goalLoc = pointFromNode(_biRRT->goalTree().rootNode());
    painter.setPen(QPen(Qt::darkGreen, 6));
    painter.drawEllipse(goalLoc, 2, 2);
}

void GridRRTWidget::drawTree(QPainter &painter,
    const Tree<Vector2f> &rrt,
    const Node<Vector2f> *solutionNode,
    QColor treeColor,
    QColor solutionColor)
{
    //  node drawing radius
    const float r = 1;

    //  draw all the nodes and connections
    for (const Node<Vector2f> *node : rrt.allNodes()) {
        painter.setPen(QPen (treeColor, 1));
        QPointF loc = pointFromNode(node);
        painter.drawEllipse(loc, r, r);

        if (node->parent()) {
            //  draw edge
            painter.setPen(QPen(treeColor, 1));
            QPointF parentLoc = pointFromNode(node->parent());
            painter.drawLine(loc, parentLoc);
        }
    }

    //  draw solution
    if (solutionNode) {
        painter.setPen(QPen(solutionColor, 2));

        const Node<Vector2f> *node = solutionNode, *parent = solutionNode->parent();
        while (parent) {
            //  draw the edge
            QPointF from = pointFromNode(node);
            QPointF to = pointFromNode(parent);
            painter.drawLine(from, to);

            //  scooch
            node = parent;
            parent = parent->parent();
        }
    }
}


#pragma mark Mouse Events

bool GridRRTWidget::mouseInGrabbingRange(QMouseEvent *event, const Vector2f &pt) {
    float dx = event->pos().x() - pt.x();
    float dy = event->pos().y() - pt.y();
    return sqrtf( dx*dx + dy*dy ) < 15;
}

void GridRRTWidget::mousePressEvent(QMouseEvent *event) {
    if (mouseInGrabbingRange(event, _biRRT->startTree().rootNode()->state())) {
        _draggingStart = true;
    } else if (mouseInGrabbingRange(event, _biRRT->goalTree().rootNode()->state())) {
        _draggingGoal = true;
    } else {
        _editingObstacles = true;
        Vector2f pos = Vector2f(event->pos().x(), event->pos().y());
        Vector2i gridLoc = _stateSpace->obstacleGrid().gridSquareForLocation(pos);
        _erasingObstacles = _stateSpace->obstacleGrid().obstacleAt(gridLoc);

        //  toggle the obstacle state of clicked square
        _stateSpace->obstacleGrid().obstacleAt(gridLoc) = !_erasingObstacles;
        update();
    }
}

void GridRRTWidget::mouseMoveEvent(QMouseEvent *event) {
    Vector2f point(event->pos().x(), event->pos().y());

    if (_draggingStart) {
        //  reset the tree with the new start pos
        _biRRT->setStartState(point);
        update();
    } else if (_draggingGoal) {
        //  set the new goal point
        _biRRT->setGoalState(point);
        update();
    } else if (_editingObstacles) {
        Vector2i gridLoc = _stateSpace->obstacleGrid().gridSquareForLocation(point);
        _stateSpace->obstacleGrid().obstacleAt(gridLoc) = !_erasingObstacles;
        update();
    }
}

void GridRRTWidget::mouseReleaseEvent(QMouseEvent *event) {
    _draggingGoal = false;
    _draggingStart = false;
    _editingObstacles = false;
}
