#ifndef VIDEO_ANNOTATOR_PLAYER_H
#define VIDEO_ANNOTATOR_PLAYER_H

#include <string>
#include <memory>
#include <atomic>
#include <map>

#include <boost/bimap.hpp>

#include <QImage>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
}

namespace fish_annotator { namespace video_annotator {

class Player : public QObject {	
    Q_OBJECT
public:
    /// @brief Constructor.
    explicit Player();

    /// @brief Destructor.
    ~Player();
public slots:
    /// @brief Plays the video.
    void play();
    
    /// @brief Stops the video.
    void stop();

    /// @brief Loads the video.
    ///
    /// @param filename Path to video.
    void loadVideo(QString filename);

    /// @brief Increases the speed of the video by a factor of two.
    void speedUp();

    /// @brief Decreases the speed of the video by a factor of two.
    void slowDown();

    /// @brief Sets position to specified frame.
    ///
    /// @param frame Frame to seek to.
    void setFrame(qint64 frame);

    /// @brief Sets position to next frame.
    void nextFrame();

    /// @brief Sets position to previous frame.
    void prevFrame();
signals:
    /// @brief Emitted when a frame is ready to display.
    //
    /// @param image Captured image.
    /// @param frame Frame number that corresponds to this image.
    void processedImage(QImage image, qint64 frame);

    /// @brief Emitted when duration changes.
    ///
    /// @param New video duration.
    void durationChanged(qint64 duration);

    /// @brief Emitted when playback rate changes.
    ///
    /// @param rate Playback rate (fps).
    void playbackRateChanged(double rate);

    /// @brief Emitted when video resolution changes.
    ///
    /// @param width Video width.
    /// @param height Video height.
    void resolutionChanged(qint64 width, qint64 height);

    /// @brief Emitted when play/pause state changed.
    ///
    /// @param stopped True if stopped, false otherwise.
    void stateChanged(bool stopped);

    /// @brief Emitted when media load starts.
    ///
    /// @param max_progress Maximum progress.
    void mediaLoadStart(int max_progress);

    /// @brief Emitted when media load progresses.
    ///
    /// @param progress Load progress.
    void loadProgress(int progress);

    /// @brief Emitted when new media is loaded.
    ///
    /// @param video_path Path to loaded video file.
    /// @param native_rate Native playback rate of the video.
    void mediaLoaded(QString video_path, qreal native_rate);

    /// @brief Emitted on error.
    ///
    /// @param err Error message.
    void error(QString err);
private:
    /// @brief Path to loaded video.
    QString video_path_;

    /// @brief Native frame rate of loaded video.
    double frame_rate_;

    /// @brief True if player is stopped, false otherwise.
    bool stopped_;

    /// @brief Stores most recent image.
    QImage image_;

    /// @brief Current playback rate.
    double current_speed_;

    /// @brief Codec context.
    AVCodecContext *codec_context_;

    /// @brief Format context.
    AVFormatContext *format_context_;

    /// @brief Packet.
    AVPacket packet_;

    /// @brief Index of video stream.
    int stream_index_;

    /// @brief Most recent frame.
    AVFrame *frame_;

    /// @brief For converting decoded frame to RGB.
    AVFrame *frame_rgb_;

    /// @brief Conversion context.
    SwsContext *sws_context_;

    /// @brief Delay between frames in microseconds.
    double delay_;

    /// @brief Last decoded frame.
    qint64 dec_frame_;

    /// @brief Last requested frame.
    qint64 req_frame_;

    /// @brief Map between frame index and decompression timestamp.
    boost::bimap<qint64, qint64> seek_map_;

    /// @brief Frame buffer.
    std::map<qint64, QImage> frame_buffer_;

    /// @brief Mutex for grabbing frames.
    QMutex frame_mutex_;

    /// @brief True when buffering.
    std::atomic<bool> buffering_;

    /// @brief Wait condition for deletion.
    QWaitCondition condition_;

    /// @brief Processes a single frame.
    void getOneFrame();

    /// @brief Sets the current frame.
    ///
    /// @param frame_num Set to this frame. Bounded by this function.
    void setCurrentFrame(qint64 frame_num);

    /// @brief Buffers frames.
    ///
    /// @param frame_num Buffer up to this frame number.
    /// @param wait Number of usec to wait between decoding frames.
    void buffer(qint64 frame_num, qint64 wait = 0);

    /// @brief Waits for specified time while allowing events to process.
    void processWait(qint64 usec);
};

}} // namespace fish_annotator::video_annotator

#endif // VIDEO_ANNOTATOR_PLAYER_H