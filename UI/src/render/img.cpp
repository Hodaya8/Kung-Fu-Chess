#include "render/img.hpp"
#include <iostream>
#include <stdexcept>

Img::Img() {
    // Constructor - img is automatically initialized as empty
}

Img& Img::read(const std::string& path,
               const std::pair<int, int>& size,
               bool keep_aspect,
               int interpolation) {
    img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty()) {
        throw std::runtime_error("Cannot load image: " + path);
    }

    if (size.first != 0 && size.second != 0) {  // Check if size is not empty
        int target_w = size.first;
        int target_h = size.second;
        int h = img.rows;
        int w = img.cols;

        if (keep_aspect) {
            double scale = std::min(static_cast<double>(target_w) / w, 
                                   static_cast<double>(target_h) / h);
            int new_w = static_cast<int>(w * scale);
            int new_h = static_cast<int>(h * scale);
            cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
        } else {
            cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
        }
    }

    return *this;
}

void Img::draw_on(Img& other_img, int x, int y) {
    if (img.empty() || other_img.img.empty()) {
        throw std::runtime_error("Both images must be loaded before drawing.");
    }

    cv::Mat source_img = img;
    cv::Mat& target_img = other_img.img;

    int h = source_img.rows;
    int w = source_img.cols;
    int H = target_img.rows;
    int W = target_img.cols;

    if (y + h > H || x + w > W) {
        throw std::runtime_error("Image does not fit at the specified position.");
    }

    // אם התמונה שלנו לא שקופה (3 ערוצים או פחות), פשוט מעתיקים אותה
    if (source_img.channels() <= 3) {
        cv::Mat roi = target_img(cv::Rect(x, y, w, h));
        if (source_img.channels() == target_img.channels()) {
            source_img.copyTo(roi);
        }
        return;
    }

    // אם לתמונה יש ערוץ שקיפות (4 ערוצים - Alpha)
    int target_channels = target_img.channels();
    
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            // קריאת הפיקסל של הכלי (התמונה המודבקת)
            cv::Vec4b source_pixel = source_img.at<cv::Vec4b>(row, col);
            double alpha = source_pixel[3] / 255.0; // חישוב אחוז השקיפות (0.0 עד 1.0)

            if (alpha > 0) { // מציירים רק איפה שלא שקוף לגמרי
                if (target_channels == 3) { // הלוח הוא תמונת BGR
                    cv::Vec3b& target_pixel = target_img.at<cv::Vec3b>(y + row, x + col);
                    // מיזוג הצבעים: צבע הכלי כפול אחוז השקיפות + צבע הלוח כפול החלק הנותר
                    target_pixel[0] = source_pixel[0] * alpha + target_pixel[0] * (1.0 - alpha);
                    target_pixel[1] = source_pixel[1] * alpha + target_pixel[1] * (1.0 - alpha);
                    target_pixel[2] = source_pixel[2] * alpha + target_pixel[2] * (1.0 - alpha);
                } else if (target_channels == 4) { // הלוח הוא תמונת BGRA
                    cv::Vec4b& target_pixel = target_img.at<cv::Vec4b>(y + row, x + col);
                    target_pixel[0] = source_pixel[0] * alpha + target_pixel[0] * (1.0 - alpha);
                    target_pixel[1] = source_pixel[1] * alpha + target_pixel[1] * (1.0 - alpha);
                    target_pixel[2] = source_pixel[2] * alpha + target_pixel[2] * (1.0 - alpha);
                }
            }
        }
    }
}

void Img::put_text(const std::string& txt, int x, int y, double font_size,
                   const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

void Img::show() {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::imshow("Image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
} 