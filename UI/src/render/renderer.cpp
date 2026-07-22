#include "render/renderer.hpp"

#include <iostream>

namespace
{
    constexpr int BOARD_SIZE = 8;
    constexpr int TOTAL_FRAMES = 5;
    constexpr int SELECTION_BORDER_SIZE = 4;

    constexpr int SCORE_MARGIN = 10;
    constexpr int SCORE_TEXT_Y = 35;
    constexpr double SCORE_FONT_SCALE = 0.8;
    constexpr int SCORE_TEXT_THICKNESS = 2;
    constexpr int SCORE_OUTLINE_THICKNESS = 4;

    const cv::Scalar SCORE_TEXT_COLOR(
        0,
        255,
        255
    );

    const cv::Scalar SCORE_OUTLINE_COLOR(
        0,
        0,
        0
    );

    void drawScoreText(
        cv::Mat frame,
        const std::string& text,
        const cv::Point& position)
    {
        // ציור מסגרת שחורה מסביב לטקסט
        cv::putText(
            frame,
            text,
            position,
            cv::FONT_HERSHEY_SIMPLEX,
            SCORE_FONT_SCALE,
            SCORE_OUTLINE_COLOR,
            SCORE_OUTLINE_THICKNESS
        );

        // ציור הטקסט עצמו
        cv::putText(
            frame,
            text,
            position,
            cv::FONT_HERSHEY_SIMPLEX,
            SCORE_FONT_SCALE,
            SCORE_TEXT_COLOR,
            SCORE_TEXT_THICKNESS
        );
    }
}

Renderer::Renderer(
    const std::string& assetsDir)
    : cellSize(100)
{
    boardPath =
        assetsDir +
        "/board_classic.png";

    const std::string basePath =
        assetsDir +
        "/pieces_classic/pieces_classic/";

    const char pieceTypes[] = {
        'Q',
        'K',
        'R',
        'B',
        'N',
        'P'
    };

    for (char type : pieceTypes)
    {
        piecePaths[
            std::string("w") + type
        ] =
            basePath + type + "W";

        piecePaths[
            std::string("b") + type
        ] =
            basePath + type + "B";
    }
}

void Renderer::render(
    const RenderModel& renderModel,
    bool gameOver,
    int whiteScore,
    int blackScore,
    const std::optional<Position>& selectedPosition)
{
    try
    {
        static const std::map<
            PieceState,
            std::string
        > stateToString = {
            {
                PieceState::idle,
                "idle"
            },
            {
                PieceState::move,
                "move"
            },
            {
                PieceState::jump,
                "jump"
            },
            {
                PieceState::long_rest,
                "long_rest"
            },
            {
                PieceState::short_rest,
                "short_rest"
            },
            {
                PieceState::captured,
                "captured"
            }
        };

        const int boardPixelSize =
            BOARD_SIZE * cellSize;

        Img frame;

        frame.read(
            boardPath,
            std::make_pair(
                boardPixelSize,
                boardPixelSize
            ),
            false
        );

        for (const auto& piece :
             renderModel.getPieces())
        {
            const auto pathIterator =
                piecePaths.find(
                    piece.type
                );

            if (pathIterator ==
                piecePaths.end())
            {
                continue;
            }

            const std::string stateString =
                stateToString.at(
                    piece.state
                );

            const int currentFrame =
                1 +
                static_cast<int>(
                    piece.animationProgress *
                    (TOTAL_FRAMES - 1)
                );

            const std::string imagePath =
                pathIterator->second +
                "/states/" +
                stateString +
                "/sprites/" +
                std::to_string(
                    currentFrame
                ) +
                ".png";

            Img pieceImage;

            try
            {
                pieceImage.read(
                    imagePath,
                    std::make_pair(
                        cellSize,
                        cellSize
                    ),
                    true
                );
            }
            catch (...)
            {
                const std::string fallbackPath =
                    pathIterator->second +
                    "/states/idle/sprites/1.png";

                pieceImage.read(
                    fallbackPath,
                    std::make_pair(
                        cellSize,
                        cellSize
                    ),
                    true
                );
            }

            const int drawX =
                static_cast<int>(
                    piece.col * cellSize
                );

            const int drawY =
                static_cast<int>(
                    piece.row * cellSize
                );

            pieceImage.draw_on(
                frame,
                drawX,
                drawY
            );

            // ציור מסגרת סביב כלי שנבחר
            if (selectedPosition.has_value())
            {
                if (
                    static_cast<int>(
                        piece.row
                    ) ==
                        selectedPosition
                            ->getRow() &&
                    static_cast<int>(
                        piece.col
                    ) ==
                        selectedPosition
                            ->getCol()
                )
                {
                    cv::rectangle(
                        frame.get_mat(),
                        cv::Point(
                            drawX,
                            drawY
                        ),
                        cv::Point(
                            drawX + cellSize,
                            drawY + cellSize
                        ),
                        cv::Scalar(
                            0,
                            255,
                            255
                        ),
                        SELECTION_BORDER_SIZE
                    );
                }
            }

            const bool isResting =
                piece.state ==
                    PieceState::short_rest ||
                piece.state ==
                    PieceState::long_rest;

            if (isResting &&
                piece.timeRemaining >= 0)
            {
                const std::string timeText =
                    std::to_string(
                        piece.timeRemaining /
                        100
                    );

                cv::putText(
                    frame.get_mat(),
                    timeText,
                    cv::Point(
                        drawX + 10,
                        drawY + 30
                    ),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.7,
                    cv::Scalar(
                        0,
                        255,
                        255
                    ),
                    2
                );
            }
        }

        if (gameOver)
        {
            cv::rectangle(
                frame.get_mat(),
                cv::Point(0, 0),
                cv::Point(
                    boardPixelSize,
                    boardPixelSize
                ),
                cv::Scalar(0, 0, 0),
                -1
            );

            cv::putText(
                frame.get_mat(),
                "GAME OVER",
                cv::Point(150, 400),
                cv::FONT_HERSHEY_DUPLEX,
                3,
                cv::Scalar(0, 0, 255),
                5
            );
        }

        const std::string whiteScoreText =
            "White: " +
            std::to_string(
                whiteScore
            );

        const std::string blackScoreText =
            "Black: " +
            std::to_string(
                blackScore
            );

        int textBaseline = 0;

        const cv::Size blackTextSize =
            cv::getTextSize(
                blackScoreText,
                cv::FONT_HERSHEY_SIMPLEX,
                SCORE_FONT_SCALE,
                SCORE_TEXT_THICKNESS,
                &textBaseline
            );

        drawScoreText(
            frame.get_mat(),
            whiteScoreText,
            cv::Point(
                SCORE_MARGIN,
                SCORE_TEXT_Y
            )
        );

        drawScoreText(
            frame.get_mat(),
            blackScoreText,
            cv::Point(
                boardPixelSize -
                    blackTextSize.width -
                    SCORE_MARGIN,
                SCORE_TEXT_Y
            )
        );

        cv::imshow(
            "Image",
            frame.get_mat()
        );
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "[UI] Render error: "
            << exception.what()
            << std::endl;
    }
}