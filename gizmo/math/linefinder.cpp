#include "linefinder.h"

using namespace std;


const static float QUADRANT_LEN = 60.f;
const static float MAX_LINE_SLOPE = 10.f;


LineFinder::LineFinder(float maxError, float maxDistance) :
	m_bestPointsNo(0),
	m_maxError(maxError),
	m_maxDistance(maxDistance),
	m_minX(HUGE_VALF),
	m_maxX(-HUGE_VALF)
{
}

bool LineFinder::addPoint(const Point &point)
{
	if (m_points.count(point))
		return false;

	bool newBestLine = false;
	const float maxErrorSquared = m_maxError * m_maxError;

	for (const Point &p1 : m_points)
	{
		Line line(p1, point);

		// line must be monotonically increasing
		if (line.a < 1.f / MAX_LINE_SLOPE || line.a > MAX_LINE_SLOPE)
			continue;

		if (point.x < m_minX)
			m_minX = point.x;
		if (point.x > m_maxX)
			m_maxX = point.x;

		if (std::abs(line.getY(m_minX) - m_minX) > m_maxDistance)
			continue;
		if (std::abs(line.getY(m_maxX) - m_maxX) > m_maxDistance)
			continue;

		size_t pointsNo = 1;

		for (const auto &col : m_quadrants)
		{
			const int qx = col.first;

			const float y1 = line.getY(qx * QUADRANT_LEN);
			const float y2 = line.getY((qx + 1) * QUADRANT_LEN);
			const int qy1 = (y1 - m_maxError) / QUADRANT_LEN;
			const int qy2 = (y2 + m_maxError) / QUADRANT_LEN;

			const auto &row = col.second;
			auto it = row.lower_bound(qy1);

			while (it != row.end() && it->first <= qy2)
			{
				for (const Point &p2 : it->second)
				{
					if (line.getDistanceSqr(p2) <= maxErrorSquared)
						pointsNo++;
				}

				++it;
			}
		}

		if (pointsNo > m_bestPointsNo)
		{
			m_bestLine = line;
			m_bestPointsNo = pointsNo;
			newBestLine = true;

			// since only one point was added, pointsNo could be only one
			// point larger than m_bestPointsNo, so there is no need to check
			// other possible lines
			break;
		}
	}

	m_points.insert(point);

	const int qx = floor(point.x / QUADRANT_LEN);
	const int qy = floor(point.y / QUADRANT_LEN);
	m_quadrants[qx][qy].push_back(point);

	return newBestLine;
}

bool LineFinder::addPoint(float x, float y)
{
	return addPoint(Point(x, y));
}

const Line &LineFinder::getBestLine() const
{
	return m_bestLine;
}

const Points &LineFinder::getPoints() const
{
	return m_points;
}

size_t LineFinder::getAlignedPointsNo() const
{
	return m_bestPointsNo;
}
