#ifndef LIBRARY_H
#define LIBRARY_H

#include <QDialog>
#include <QDateTime>
#include <QDebug>
#include <QTimer>

#define MAX_INPUT_CHAR 255
#define MSG_PER_30_SEC 4
#define TIME_30_SEC 30 * 1000
#define TIME_PREVENT_SPAM 3 * 1000

#define USERNAME_REGEX "^[a-zA-Z0-9]{8,20}$"
#define PASS_REGEX "^[a-zA-Z0-9!@#$%^&*()]{8,20}$"

#endif // LIBRARY_H
