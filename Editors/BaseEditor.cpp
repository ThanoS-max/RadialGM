#include "BaseEditor.h"

#include "Components/Logger.h"

#include <QCloseEvent>
#include <QMessageBox>

BaseEditor::BaseEditor(EditorModel* model, QWidget* parent)
    : QWidget(parent), _model(model) {
  _model->setParent(this); // << take ownership
  _model->revert(); // << prepare initial backup
  _mapper = new EditorMapper(_model, this);

  // the editor only becomes modified if it was edited
  // through its editor model because that is the only
  // case in which it is able to restore from a backup
  auto markDirty = [this]() {
    if (QObject::sender() != _model) return; // << ignore all else/super model
    this->setWindowModified(true);
  };
  // handle fields being changed
  connect(_model, &EditorModel::dataChanged, markDirty);
  // handle repeated fields being changed (e.g, insert/move/remove)
  connect(_model, &EditorModel::rowsInserted, markDirty);
  connect(_model, &EditorModel::rowsMoved, markDirty);
  connect(_model, &EditorModel::rowsRemoved, markDirty);
}

void BaseEditor::closeEvent(QCloseEvent* event) {
  if (isWindowModified()) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Resource Changed"),
                                  tr("Do you want to keep the changes?"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
      event->ignore();
      return;
    } else if (reply == QMessageBox::No) {
      _mapper->clearMapping(); // << mapper does not give a FUCK about changes
      // This should never happen but here incase someone decides to incorrectly null the backup
      R_ASSESS(_model->submit()) << "Failed to revert editor changes:"
                                 << this->windowTitle();
    }
  }

  // this fixes a bug? in QMdiSubWindow destructor where it
  // indiscriminately clears the ancestor window modified
  // https://bugreports.qt.io/browse/QTBUG-85924
  // for now we workaround by resetting it right after it's destroyed
  auto ancestorWindow = window();
  bool wasMainWindowModified = ancestorWindow->isWindowModified();
  connect(this, &QObject::destroyed, [ancestorWindow,wasMainWindowModified]() {
    ancestorWindow->setWindowModified(wasMainWindowModified);
  });

  event->accept();
}

void BaseEditor::OnSave() {
  // our editor model already mutated the super model so we
  // don't actually have to do anything and can just close
  this->parentWidget()->close();
}
