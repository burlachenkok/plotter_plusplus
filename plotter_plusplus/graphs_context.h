#ifndef GRAPHS_CONTEXT_H
#define GRAPHS_CONTEXT_H

#include <QtCharts/QXYSeries>
#include <vector>
#include <assert.h>

QT_CHARTS_USE_NAMESPACE

struct GraphsContext
{
    struct Plot2D
    {
        Plot2D()
        : visible(true)
        , plotDotLine(false)
        , widthInPixels(1.0f)
        {
            rgbColor[0] = 255;
            rgbColor[1] = 0;
            rgbColor[2] = 0;
        }

        std::string name;
        std::vector<double> xCoord;
        std::vector<double> yCoord;
        unsigned char rgbColor[3];

        bool visible;
        bool plotDotLine;
        float widthInPixels;

        Plot2D& add(double x, double y) {
            xCoord.push_back(x);
            yCoord.push_back(y);
            return *this;
        }

        Plot2D& clean() {
            xCoord.clear();
            yCoord.clear();
            return *this;
        }

        Plot2D& removeFirstNPoint(int n) {
            xCoord.erase(xCoord.begin(), xCoord.begin() + n);
            yCoord.erase(yCoord.begin(), yCoord.begin() + n);
            return *this;
        }

        size_t size() const {
            assert(xCoord.size() == yCoord.size());
            return xCoord.size();
        }
    };

    std::vector<Plot2D> graphics;
    std::string consoleLog;

    int findPlotByName(const std::string& name) const
    {
        for (size_t i = 0; i < graphics.size(); ++i)
        {
            if (graphics[i].name == name)
                return static_cast<int>(i);
        }
        return -1;
    }
}; 

inline GraphsContext& graphContext()
{
    static GraphsContext g;
    return g;
}

#endif // GRAPHS_CONTEXT_H
