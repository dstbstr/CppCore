#pragma once
#include <optional>
#include <array>
#include "Core/Platform/Types.h"
#include "ConstexprMatrix.h"

// Integer Linear Programming (ILP)
namespace _IlpImpl {
	template<size_t Rows, size_t Cols>
	constexpr s32 MaxPossible(const Mat<s32, Rows, Cols>& A, size_t col, const std::array<s32, Rows>& rhs) {
		auto maxPresses = *std::max_element(rhs.begin(), rhs.end());
		s32 result = maxPresses;
		for (size_t row = 0; row < Rows; row++) {
			if (A[row][col] > 0) {
				result = std::min(result, rhs[row] / A[row][col]);
			}
		}
		return result;
	}

	constexpr s32 MaxPossible(const std::vector<std::vector<s32>>& A, size_t col, const std::vector<s32>& rhs) {
		auto maxPresses = *std::max_element(rhs.begin(), rhs.end());
		s32 result = maxPresses;
		for (size_t row = 0; row < A.size(); row++) {
			if (A[row][col] > 0) {
				result = std::min(result, rhs[row] / A[row][col]);
			}
		}
		return result;
	}

	template<size_t Rows, size_t Cols>
	constexpr void Search(
		const Mat<s32, Rows, Cols>& A, 
		size_t col, 
		std::array<s32, Rows> rhs,
		std::array<s32, Cols> x,
		s32 currentSum,
		s32& outBestSum, 
		std::array<s32, Cols>& outBest) {
		if (col == Cols) {
			if (rhs == std::array<s32, Rows>{} && currentSum < outBestSum) {
				outBestSum = currentSum;
				outBest = x;
			}
			return;
		}
		s32 maxVal = MaxPossible(A, col, rhs);
		//for (s32 v = 0; v <= maxVal; v++) {
		for(auto v = maxVal; v >= 0; v--) {
			x[col] = v;
			auto newRhs = rhs;
			for (size_t row = 0; row < Rows; row++) {
				newRhs[row] -= A[row][col] * v;
			}
			bool valid = true;
			for (size_t row = 0; row < Rows; row++) {
				if (newRhs[row] < 0) {
					valid = false;
					break;
				}
			}
			if (!valid || currentSum + v >= outBestSum) continue;
			Search(A, col + 1, newRhs, x, currentSum + v, outBestSum, outBest);
		}
	};

	constexpr void Search(
		const std::vector<std::vector<s32>>& A,
		size_t col,
		std::vector<s32> target,
		std::vector<s32> solution,
		s32 currentSum,
		s32& outBestSum,
		std::vector<s32>& outBest) {
		if (col == solution.size()) {
			if (target == std::vector<s32>(target.size(), 0) && currentSum < outBestSum) {
				outBestSum = currentSum;
				outBest = solution;
			}
			return;
		}
		s32 maxVal = MaxPossible(A, col, target);
		for (s32 v = maxVal; v >= 0; v--) {
		//for (s32 v = 0; v <= maxVal; v++) {
			solution[col] = v;
			auto newTarget = target;
			for (size_t row = 0; row < target.size(); row++) {
				newTarget[row] -= A[row][col] * v;
			}
			bool valid = true;
			for (size_t row = 0; row < newTarget.size(); row++) {
				if (newTarget[row] < 0) {
					valid = false;
					break;
				}
			}
			if (!valid || currentSum + v >= outBestSum) continue;
			int lowerBound = currentSum + v;
			for(size_t i = 0; i < newTarget.size(); i++) {
				if(newTarget[i] > 0) {
					s32 maxA = 0;
					for(size_t j = col + 1; j < solution.size(); j++) {
						maxA = std::max(maxA, A[i][j]);
					}
					if(maxA == 0) {
						lowerBound = std::numeric_limits<s32>::max();
						break;
					}
					lowerBound += (newTarget[i] + maxA - 1) / maxA;
				}
			}
			if (lowerBound >= outBestSum) continue;

			Search(A, col + 1, newTarget, solution, currentSum + v, outBestSum, outBest);
		}
	};

}
template<size_t Rows, size_t Cols>
constexpr std::optional<std::array<s32, Cols>> SolveIlp(const Mat<s32, Rows, Cols>& A, const std::array<s32, Rows>& b) {
	std::array<s32, Cols> x{};

	s32 bestSum = std::numeric_limits<s32>::max();
	std::array<s32, Cols> best{};
	_IlpImpl::Search(A, 0, b, x, 0, bestSum, best);
	if(bestSum == std::numeric_limits<s32>::max()) {
		return std::nullopt;
	}
	return best;
}

constexpr std::optional<std::vector<s32>> SolveIlp(const std::vector<std::vector<s32>>& A, const std::vector<s32>& target) {
	size_t rowCount = A.size();
	size_t colCount = A[0].size();
	if(target.size() != rowCount) {
		throw "The target vector must be the same size as the rows in the matrix";
	}

	std::vector<s32> solution(colCount, 0);
	s32 bestSum = std::numeric_limits<s32>::max();
	std::vector<s32> best;
	_IlpImpl::Search(A, 0, target, solution, 0, bestSum, best);
	if (bestSum == std::numeric_limits<s32>::max()) {
		return std::nullopt;
	}
	return best;
}

namespace Ilp {
	namespace _Impl {
		struct Table {
			std::vector<std::vector<double>> Data;
			size_t ObjRow;
			size_t NumCols;
			size_t RhsCol;
			size_t OrigVarCount;

			template<typename T>
			constexpr Table(std::vector<std::vector<T>> matrix, std::vector<T> target, bool minimize, bool phase1) 
			{
				OrigVarCount = matrix[0].size();
				size_t m = matrix.size();
				size_t n = matrix[0].size();
				ObjRow = m;

				if(phase1) {
					RhsCol = n + m;
					NumCols = n + m + 1;
					Data = std::vector<std::vector<double>>(m + 1, std::vector<double>(NumCols, 0.0));
					for(size_t i = 0; i < m; i++) {
						for(size_t j = 0; j < n; j++) {
							Data[i][j] = static_cast<double>(matrix[i][j]);
						}
						Data[i][n + i] = 1.0; // artificial variable
						Data[i][RhsCol] = static_cast<double>(target[i]);
					}
					for(size_t j = n; j < n + m; j++) {
						Data[ObjRow][j] = 1.0; // minimize sum of artificial variables
					}
				}
				else {
					// phase 2
					RhsCol = n;
					NumCols = n + 1;
					Data = std::vector<std::vector<double>>(m + 1, std::vector<double>(NumCols, 0.0));
					// Constraints
					for (size_t i = 0; i < ObjRow; i++) {
						for (size_t j = 0; j < RhsCol; j++) {
							Data[i][j] = static_cast<double>(matrix[i][j]);
						}
						Data[i][RhsCol] = static_cast<double>(target[i]);
					}

					// Objectives
					for (size_t j = 0; j < NumCols; j++) {
						Data[ObjRow][j] = minimize ? 1.0 : -1.0;
					}
				}
			}
		};
		constexpr bool IsOptimal(const Table& table) {
			auto IsNegative = [](double val) { return val < 0.0; };
			const auto& row = table.Data[table.ObjRow];
			return !std::any_of(row.begin(), row.begin() + table.RhsCol, IsNegative);
		}
		constexpr size_t GetPivotColumn(const Table& table) {
			size_t result = table.RhsCol;
			double min = 0.0;
			for(size_t col = 0; col < table.RhsCol; col++) {
				double val = table.Data[table.ObjRow][col];
				if(val < min) {
					min = val;
					result = col;
				}
			}
			return result;
		}

		constexpr size_t GetPivotRow(const Table& table, size_t pivotCol) {
			size_t result = table.ObjRow;
			double minRatio = std::numeric_limits<double>::max();

			for(size_t row = 0; row < table.ObjRow; row++) {
				double entry = table.Data[row][pivotCol];
				if (entry <= 0.0) continue;
				double rhs = table.Data[row][table.RhsCol];
				double ratio = rhs / entry;
				if(ratio < minRatio) {
					minRatio = ratio;
					result = row;
				}
			}
			return result;
		}

		constexpr void PerformPivot(Table& table, size_t pivotRow, size_t pivotCol) {
			auto rowCount = table.Data.size();
			auto colCount = table.Data[0].size();
			auto pivotVal = table.Data[pivotRow][pivotCol];

			// Normalize pivot row
			for(size_t col = 0; col < colCount; col++) {
				table.Data[pivotRow][col] /= pivotVal;
			}

			// Eliminate the pivot column in other rows
			for(size_t row = 0; row < rowCount; row++) {
				if(row == pivotRow) continue;
				auto factor = table.Data[row][pivotCol];
				for(size_t col = 0; col < colCount; col++) {
					table.Data[row][col] -= factor * table.Data[pivotRow][col];
				}
			}
		}

		constexpr std::vector<double> Solve(Table& table) {
			std::vector<double> solution(table.RhsCol, 0.0);
			for(size_t col = 0; col < table.RhsCol; col++) {
				size_t basicRow = table.ObjRow; // not-found
				for(size_t row = 0; row < table.ObjRow; row++) {
					if (table.Data[row][col] != 1.0) continue;
					bool isBasic = true;
					for(size_t otherRow = 0; otherRow < table.ObjRow; otherRow++) {
						if (otherRow == row) continue;
						if (table.Data[otherRow][col] != 0.0) {
							isBasic = false;
							break;
						}
					}
					if (isBasic) {
						basicRow = row;
						break;
					}
				}
				if(basicRow != table.ObjRow) {
					solution[col] = table.Data[basicRow][table.RhsCol];
				}
			}
			return solution;
		}
	}
	template<typename T>
	constexpr std::vector<T> SimplexMin(const std::vector<std::vector<T>>& A, const std::vector<T>& target) {
		_Impl::Table phase1Table(A, target, true, true);

		_Impl::Table phase2Table = phase1Table; // TODO: Fix
		while(!_Impl::IsOptimal(phase2Table)) {
			size_t pivotCol = _Impl::GetPivotColumn(phase2Table);
			size_t pivotRow = _Impl::GetPivotRow(phase2Table, pivotCol);
			_Impl::PerformPivot(phase2Table, pivotRow, pivotCol);
		}
		auto solution = _Impl::Solve(phase2Table);
		std::vector<T> result(target.size(), 0);
		for (size_t i = 0; i < solution.size(); i++) {
			// verify integer
			result[i] = static_cast<T>(solution[i]);
		}

		return result;
	}

	constexpr bool TestIlp() {
		std::vector<std::vector<s32>> A = {
		{ 0, 0, 0, 0, 1, 1 },
		{ 0, 1, 0, 0, 0, 1 },
		{ 0, 0, 1, 1, 1, 0 },
		{ 1, 1, 0, 1, 0, 0 }
		};
		std::vector<s32> target = { 3, 5, 4, 7 };
		auto solution = SolveIlp(A, target);
		if(!solution.has_value()) {
			return false;
		}
		auto sum = std::accumulate(solution->begin(), solution->end(), 0);
		return sum == 10;
	}
	/*
	inline std::vector<std::string> PrintTable() {
		std::vector<std::vector<s32>> A = {
			{ 0, 0, 0, 0, 1, 1 },
			{ 0, 1, 0, 0, 0, 1 },
			{ 0, 0, 1, 1, 1, 0 },
			{ 1, 1, 0, 1, 0, 0 }
		};
		std::vector<s32> target = { 3, 5, 4, 7 };

		auto table = _Impl::Table(A, target, true, true);
		std::vector<std::string> lines;
		for (const auto& row : table.Data) {
			std::string line;
			for (const auto& val : row) {
				line += std::to_string(static_cast<size_t>(val)) + "\t";
			}
			lines.push_back(line);
		}
		return lines;
	}
	*/
	/*
	constexpr bool TestSimplex() {
		// (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
			
		std::vector<std::vector<s32>> A = {
			{ 0, 0, 0, 0, 1, 1 },
			{ 0, 1, 0, 0, 0, 1 },
			{ 0, 0, 1, 1, 1, 0 },
			{ 1, 1, 0, 1, 0, 0 }
		};
		std::vector<s32> target = { 3, 5, 4, 7 };
		auto solution = SimplexMin(A, target);
		return solution[0] == 1 &&
			solution[1] == 3 &&
			solution[2] == 0 &&
			solution[3] == 3 &&
			solution[4] == 1 &&
			solution[5] == 2;
	}
	*/

	//static_assert(TestSimplex());
}