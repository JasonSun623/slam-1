//
// Created by matzipan on 06/02/17.
//

#include "NetworkPlot.h"

#include "moc_NetworkPlot.cpp"


NetworkPlot::NetworkPlot() : QObject() {
    context = new zmqpp::context();

    socket = new zmqpp::socket(*context, zmqpp::socket_type::push);

    socket->connect("tcp://localhost:4242");
}

NetworkPlot::~NetworkPlot() {}

void NetworkPlot::sendXYArrays(zmqpp::message &message, QVector<double> &xs, QVector<double> &ys) {
    message<<xs.size();
    for (int i = 0; i < xs.size(); i++) {
        message << xs[i];
    }

    message<<ys.size();
    for (int i = 0; i < ys.size(); i++) {
        message << ys[i];
    }

    socket->send(message);
}

void NetworkPlot::setLandmarks(QVector<double> &xs, QVector<double> &ys) {
    zmqpp::message message;

    message<<"setLandmarks";

    sendXYArrays(message, xs, ys);
}

void NetworkPlot::setWaypoints(QVector<double> &xs, QVector<double> &ys) {
    zmqpp::message message;

    message<<"setWaypoints";

    sendXYArrays(message, xs, ys);
}

void NetworkPlot::setParticles(QVector<double> &xs, QVector<double> &ys) {
    zmqpp::message message;

    message<<"setParticles";

    sendXYArrays(message, xs, ys);
}

void NetworkPlot::setFeatureParticles(QVector<double> &xs, QVector<double> &ys) {
    zmqpp::message message;

    message<<"setFeatureParticles";

    sendXYArrays(message, xs, ys);
}

void NetworkPlot::setLaserLines(Eigen::MatrixXf &lnes) {
    zmqpp::message message;

    message<<"setLaserLines";

    message<<lnes.rows()<<lnes.cols();
    for (int i = 0; i < lnes.rows(); i++) {
        for (int j = 0; j < lnes.cols(); j++) {
            message << lnes(i, j);
        }
    }

    socket->send(message);
}

void NetworkPlot::setCovEllipse(Eigen::MatrixXf &lnes, int idx) {
    zmqpp::message message;

    message<<"setCovEllipse";

    message<<lnes.rows()<<lnes.cols();
    for (int i = 0; i < lnes.rows(); i++) {
        for (int j = 0; j < lnes.cols(); j++) {
            message << lnes(i, j);
        }
    }

    message<<idx;

    socket->send(message);
}

void NetworkPlot::addTruePosition(double x, double y) {
    zmqpp::message message;

    message<<"addTruePosition";

    message<<x<<y;

    socket->send(message);
}

void NetworkPlot::addEstimatedPosition(double x, double y) {
    zmqpp::message message;

    message<<"addEstimatedPosition";

    message<<x<<y;

    socket->send(message);
}

void NetworkPlot::setCarSize(double s, int id) {
    zmqpp::message message;

    message<<"setCarSize";

    message<<s<<id;

    socket->send(message);
}

void NetworkPlot::setCarTruePosition(double x, double y, double t) {
    zmqpp::message message;

    message<<"setCarTruePosition";

    message<<x<<y<<t;

    socket->send(message);
}

void NetworkPlot::setCarEstimatedPosition(double x, double y, double t) {
    zmqpp::message message;

    message<<"setCarEstimatedPosition";

    message<<x<<y<<t;

    socket->send(message);
}

void NetworkPlot::setPlotRange(double xmin, double xmax, double ymin, double ymax) {
    zmqpp::message message;

    message<<"setPlotRange";

    message<<xmin<<xmax<<ymin<<ymax;

    socket->send(message);
}

void NetworkPlot::clear() {
    zmqpp::message message;

    message<<"clear";

    socket->send(message);
}

void NetworkPlot::setScreenshotFilename(std::string filename) {
    zmqpp::message message;

    message<<"setScreenshotFilename"<<filename;

    socket->send(message);
}

void NetworkPlot::showMessage(QString text) {
    zmqpp::message message;

    message<<"showMessage"<<text.toStdString();

    socket->send(message);
}

void NetworkPlot::plot() {
    zmqpp::message message;

    message<<"plot";

    socket->send(message);
}

void NetworkPlot::covEllipseAdd(int n) {
    zmqpp::message message;

    message<<"covEllipseAdd"<<n;

    socket->send(message);
}

// @TODO These unimplemented functions can be used to send messages from WindowPlot to NetworkPlot when an event happens
void NetworkPlot::canvasMousePressEvent(QMouseEvent *event) {}
void NetworkPlot::keyPressEvent(QKeyEvent *event) {}
void NetworkPlot::mousePressEvent(QMouseEvent *event) {}
