#include "input/Controller.hpp"

#include <utility>

Controller::Controller(
    BoardMapper& mapper,
    MoveHandler moveHandler,
    JumpHandler jumpHandler)
    : mapper(mapper),
      moveHandler(
          std::move(moveHandler)
      ),
      jumpHandler(
          std::move(jumpHandler)
      )
{
}

bool Controller::isPlayerPiece(
    const GameSnapshot& snapshot,
    const Position& cell,
    Color playerColor) const
{
    const char colorPrefix =
        playerColor == Color::WHITE
            ? 'w'
            : 'b';

    for (const auto& piece :
         snapshot.getPieces())
    {
        if (
            piece.row != cell.getRow() ||
            piece.col != cell.getCol()
        )
        {
            continue;
        }

        return
            !piece.type.empty() &&
            piece.type.front() ==
                colorPrefix;
    }

    return false;
}

void Controller::selectCell(
    const Position& cell)
{
    selectedPosition =
        cell;
}

void Controller::handleLeftClick(
    int x,
    int y,
    const GameSnapshot& snapshot,
    Color playerColor)
{
    if (!mapper.insideBoard(
            x,
            y))
    {
        clearSelection();
        return;
    }

    const Position cell =
        mapper.pixelToCell(
            x,
            y
        );

    if (!selectedPosition.has_value())
    {
        if (isPlayerPiece(
                snapshot,
                cell,
                playerColor))
        {
            selectCell(
                cell
            );
        }

        return;
    }

    if (!isPlayerPiece(
            snapshot,
            selectedPosition.value(),
            playerColor))
    {
        clearSelection();
        return;
    }

    if (cell ==
        selectedPosition.value())
    {
        if (jumpHandler)
        {
            jumpHandler(
                cell
            );
        }

        clearSelection();
        return;
    }

    if (isPlayerPiece(
            snapshot,
            cell,
            playerColor))
    {
        selectCell(
            cell
        );

        return;
    }

    if (moveHandler)
    {
        moveHandler(
            selectedPosition.value(),
            cell
        );
    }

    clearSelection();
}

void Controller::handleRightClick(
    int x,
    int y,
    const GameSnapshot& snapshot,
    Color playerColor)
{
    if (!mapper.insideBoard(
            x,
            y))
    {
        return;
    }

    const Position cell =
        mapper.pixelToCell(
            x,
            y
        );

    if (!isPlayerPiece(
            snapshot,
            cell,
            playerColor))
    {
        return;
    }

    if (jumpHandler)
    {
        jumpHandler(
            cell
        );
    }
}

std::optional<Position>
Controller::getSelectedPosition() const
{
    return selectedPosition;
}

void Controller::clearSelection()
{
    selectedPosition.reset();
}