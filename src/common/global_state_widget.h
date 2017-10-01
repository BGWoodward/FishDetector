#ifndef GLOBAL_STATE_WIDGET_H
#define GLOBAL_STATE_WIDGET_H

#include <QWidget>
#include <QCheckBox>

#include <memory>
#include "ui_global_state_widget.h"

namespace fish_annotator {

class GlobalStateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalStateWidget(QWidget *parent = 0);

private:
    std::unique_ptr<Ui::GlobalStateWidget> ui_;

public:
    void add_checkbox(QCheckBox *checkbox);
};

} // namespace fish_annotator

#endif // GLOBAL_STATE_WIDGET_H