//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     DAVID CHOCHOLATY <xchoch09@stud.fit.vutbr.cz>
// $Date:       $2021-02-25
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author DAVID CHOCHOLATY
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

class DefaultMatrix : public ::testing::Test
{
    protected:
        Matrix matrix;
};

class SquareDefaultMatrix : public ::testing::Test
{
    protected:
        Matrix matrix = Matrix(3, 3);
};

class SquareMatrix : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            std::vector<std::vector<double>> matrix_values
            {
                {10.2, 2.4, -1.5},
                {4.5, 5.7, 2.4},
                {9.2, -7.1, 0.0}
            };

            matrix.set(matrix_values);
        }

        Matrix matrix = Matrix(3, 3);    
};

class MoreRowsMatrix : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            std::vector<std::vector<double>> matrix_values
            {
                {10.2, 2.4},
                {4.5, 5.7},
                {9.2, -7.1},
                {-1.5, 2.4},
                {0.0, 5.1}
            };

            matrix.set(matrix_values);
        }

        Matrix matrix = Matrix(5, 2);    
};

class MoreColsMatrix : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            std::vector<std::vector<double>> matrix_values
            {
                {10.2, 2.4, -1.5, 9.2, -7.1},
                {4.5, 5.7, 2.4, 0.0, 5.1}                
            };

            matrix.set(matrix_values);
        }

        Matrix matrix = Matrix(2, 5);    
};

class NegativeDeterminantMatrix : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            std::vector<std::vector<double>> matrix_values
            {
                {-10.2, 2.4, -1.5},
                {-4.5, 5.7, 2.4},
                {9.2, -7.1, 0.0}
            };

            matrix.set(matrix_values);
        }

        Matrix matrix = Matrix(3, 3);    
};

class TwoDimensionalSquareMatrix : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            std::vector<std::vector<double>> matrix_values
            {
                {10.2, -2.5},
                {4.5, 5.7}               
            };

            matrix.set(matrix_values);
        }

        Matrix matrix = Matrix(2, 2);    
};

class FourDimensionalSquareMatrix : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            std::vector<std::vector<double>> matrix_values
            {
                {-10.2, 2.4, -1.5, 5.7},
                {-4.5, 5.7, 2.4, 1.2},
                {9.2, -7.1, 0.0, 1.7},   
                {0.2, 5.1, 9.7, 1.0}
            };

            matrix.set(matrix_values);
        }

        Matrix matrix = Matrix(4, 4);    
};

//============================================================================//
// ** DEFAULT MATRIX TESTS**
//============================================================================//

TEST_F(DefaultMatrix, SetOnPositionSuccess)
{    
    EXPECT_TRUE(matrix.set(0, 0, 12.7));    
    EXPECT_TRUE(matrix.set(0, 0 , 0.0));    
    EXPECT_TRUE(matrix.set(0 ,0 , -10.5));
}

TEST_F(DefaultMatrix, SetOnPositionFailure)
{            
    EXPECT_FALSE(matrix.set(0, 1, 5.6));    
    EXPECT_FALSE(matrix.set(1, 0, -10.9));
    EXPECT_FALSE(matrix.set(5, 10, 12.2));    
}

TEST_F(DefaultMatrix, SetFromVectorSuccess)
{
    std::vector<std::vector<double>> new_matrix =
    std::vector<std::vector<double>>(1, std::vector<double>(1, 10));

    EXPECT_TRUE(matrix.set(new_matrix));
}

TEST_F(DefaultMatrix, SetFromVectorFailure)
{    
    std::vector<std::vector<double>> new_matrix
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1}
    };

    EXPECT_FALSE(matrix.set(new_matrix));
}   

TEST_F(DefaultMatrix, GetSuccess)
{
    EXPECT_EQ(matrix.get(0, 0), 0.0);
    
    ASSERT_TRUE(matrix.set(0, 0, 12.7));
    EXPECT_DOUBLE_EQ(12.7, matrix.get(0, 0));
}

TEST_F(DefaultMatrix, GetFailure)
{
    EXPECT_THROW(matrix.get(0, 1), std::runtime_error);
    EXPECT_THROW(matrix.get(1, 0), std::runtime_error);
    EXPECT_THROW(matrix.get(7, 10), std::runtime_error);
}

TEST_F(DefaultMatrix, EqualIdenticalMatrices)
{
    Matrix new_matrix;    
    EXPECT_TRUE(matrix.operator==(new_matrix));

    ASSERT_TRUE(matrix.set(0, 0, 12.7));    
    ASSERT_TRUE(new_matrix.set(0, 0, 12.7));    
    EXPECT_TRUE(matrix.operator==(new_matrix));
}

TEST_F(DefaultMatrix, EqualMismatchedMatrices)
{
    Matrix new_matrix;

    ASSERT_TRUE(new_matrix.set(0, 0, 12.7));    
    EXPECT_FALSE(matrix.operator==(new_matrix));

    ASSERT_TRUE(matrix.set(0, 0, 1.2));    
    ASSERT_TRUE(new_matrix.set(0, 0, -5.7));    
    EXPECT_FALSE(matrix.operator==(new_matrix));
}

TEST_F(DefaultMatrix, EqualDifferentSize)
{        
    Matrix new_matrix(3, 3);

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));            
    EXPECT_THROW(matrix.operator==(new_matrix), std::runtime_error);
}

TEST_F(DefaultMatrix, SumSuccess)
{
    Matrix new_matrix;
    Matrix result;
        
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));    
    EXPECT_EQ(result.get(0, 0), 0.0);

    ASSERT_TRUE(matrix.set(0, 0, 5.7)); 
    ASSERT_TRUE(new_matrix.set(0, 0, (2.2)));           
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));     
    EXPECT_DOUBLE_EQ(7.9, result.get(0, 0));        

    ASSERT_TRUE(matrix.set(0, 0, 12.7)); 
    ASSERT_TRUE(new_matrix.set(0, 0, (-2.5)));           
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));     
    EXPECT_DOUBLE_EQ(10.2, result.get(0, 0));    
}

TEST_F(DefaultMatrix, SumDifferentSize)
{
    Matrix new_matrix(2, 3);
    Matrix result;
    
    EXPECT_THROW(result = matrix.operator+(new_matrix), std::runtime_error); 
}

TEST_F(DefaultMatrix, MulMatricesSuccess)
{
    
    Matrix new_matrix;
    Matrix result;
    Matrix row_matrix(1, 10);

    ASSERT_TRUE(new_matrix.set(0, 0, 12.7));   
    ASSERT_NO_THROW(result = matrix.operator*(new_matrix));
    EXPECT_EQ(result.get(0, 0), 0.0);        

    ASSERT_TRUE(matrix.set(0, 0, -10.2));   
    ASSERT_TRUE(new_matrix.set(0, 0, 1.1));   
    ASSERT_NO_THROW(result = matrix.operator*(new_matrix));
    EXPECT_DOUBLE_EQ(-11.22, result.get(0, 0));        

    std::vector<std::vector<double>> matrix_values
    {        
        {-5.2, 7.0, 2.2, 2.8, 0.8, -9.2, -0.2, 0.0 , 8.51, -1.0}
    };

    ASSERT_TRUE(row_matrix.set(matrix_values));  
    ASSERT_TRUE(matrix.set(0, 0, 1.5));       
    ASSERT_NO_THROW(result = matrix.operator*(row_matrix));

    double cmp[] = {-7.8, 10.5, 3.3, 4.2, 1.2, -13.8, -0.3, 0.0, 12.765, -1.5};
    double value;
    size_t cmp_length = *(&cmp + 1) - cmp;

    for(size_t i = 0; i < cmp_length; i++)
    {
        ASSERT_NO_THROW(value = result.get(0, i));
        EXPECT_DOUBLE_EQ(cmp[i], value);
    }      
}

TEST_F(DefaultMatrix, MulMatricesFailure)
{
    Matrix new_matrix(5, 1);
    EXPECT_THROW(matrix.operator*(new_matrix), std::runtime_error);
}

TEST_F(DefaultMatrix, MulScalar)
{
    Matrix result;
    double scalar = 2.1;

    result = matrix.operator*(scalar);

    EXPECT_EQ(result.get(0,0), 0.0);

    ASSERT_TRUE(matrix.set(0, 0, -10.2125));   
    result = matrix.operator*(scalar);
    EXPECT_DOUBLE_EQ(-21.44625, result.get(0,0));
}

TEST_F(DefaultMatrix, SolveEquationSuccess)
{
    std::vector<double> right_side{6.25};
    ASSERT_TRUE(matrix.set(0, 0, 1.25));   

    EXPECT_DOUBLE_EQ(5.0, matrix.solveEquation(right_side)[0]);    

    right_side[0] = 0.0;
    EXPECT_DOUBLE_EQ(0.0, matrix.solveEquation(right_side)[0]);    
}

TEST_F(DefaultMatrix, SolveEquationFailure)
{    
    std::vector<double> right_side{0.0};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
    
    right_side[0] = 6.25;      
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);

    right_side.resize(2, 7.12);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
}

TEST_F(DefaultMatrix, TransposeMatrix)
{
    Matrix result;

    result = matrix.transpose();
    EXPECT_EQ(result.get(0,0), 0);

    ASSERT_TRUE(matrix.set(0, 0, 7.521)); 
    result = matrix.transpose();
    EXPECT_EQ(result.get(0,0), 7.521);
}

TEST_F(DefaultMatrix, InverseMatrixDifferentSize)
{
    EXPECT_THROW(matrix.inverse(), std::runtime_error);
}

TEST_F(DefaultMatrix, ZeroRowsMatrix)
{
    EXPECT_THROW(matrix = Matrix(0, 1), std::runtime_error);
}

TEST_F(DefaultMatrix, ZeroColsMatrix)
{
    EXPECT_THROW(matrix = Matrix(1, 0), std::runtime_error);
}

//============================================================================//
// ** SQUARE DEFAULT MATRIX TESTS**
//============================================================================//

TEST_F(SquareDefaultMatrix, SetOnPositionSuccess)
{    
    EXPECT_TRUE(matrix.set(0, 0, 12.7));    
    EXPECT_TRUE(matrix.set(2, 2, 0.0));        
    EXPECT_TRUE(matrix.set(1, 2, -7.2));
    EXPECT_TRUE(matrix.set(2, 0, 5.7));
}

TEST_F(SquareDefaultMatrix, SetOnPositionFailure)
{            
    EXPECT_FALSE(matrix.set(0, 3, -10.5));
    EXPECT_FALSE(matrix.set(3, 0, 0.5));
    EXPECT_FALSE(matrix.set(5, 10, -7.2));
    EXPECT_FALSE(matrix.set(15, 10, 12.1));
}

TEST_F(SquareDefaultMatrix, SetFromVectorSuccess)
{
    std::vector<std::vector<double>> new_matrix
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1}
    };

    EXPECT_TRUE(matrix.set(new_matrix));
}

TEST_F(SquareDefaultMatrix, SetFromVectorFailure)
{    
    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1, 12.1},
        {4.7, -2.6, 7.9, 9.57},
        {-5.2, 7.0, 9.1, 4.15}
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1},
        {12.1, 9.5, 4.5} 
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0}
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},        
    };

    EXPECT_FALSE(matrix.set(more_cols));
    EXPECT_FALSE(matrix.set(more_rows));
    EXPECT_FALSE(matrix.set(less_cols));
    EXPECT_FALSE(matrix.set(less_rows));
}   

TEST_F(SquareDefaultMatrix, GetSuccess)
{        
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
    };

    rows_count = matrix_values.size();    

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = matrix_values[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(matrix.get(i, j), matrix_values[i][j]);        
        }
    }        
}

TEST_F(SquareDefaultMatrix, GetFailure)
{
    EXPECT_THROW(matrix.get(0, 3), std::runtime_error);
    EXPECT_THROW(matrix.get(3, 0), std::runtime_error);
    EXPECT_THROW(matrix.get(7, 10), std::runtime_error);
    EXPECT_THROW(matrix.get(17, 5), std::runtime_error);
}

TEST_F(SquareDefaultMatrix, EqualIdenticalMatrices)
{
    Matrix new_matrix(3, 3);    
    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_TRUE(matrix.operator==(new_matrix));
}

TEST_F(SquareDefaultMatrix, EqualMismatchedMatrices)
{
   Matrix new_matrix(3, 3);    
    std::vector<std::vector<double>> matrix_values
    {
        {0.2, 2.4, 1.5},
        {4.5, 5.17, 2.4},
        {9.02, -7.1, 0.1}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_FALSE(matrix.operator==(new_matrix));
}

TEST_F(SquareDefaultMatrix, EqualDifferentSize)
{        
    Matrix m_more_cols(3, 4);
    Matrix m_more_rows(4, 3);
    Matrix m_less_cols(3, 2);
    Matrix m_less_rows(2, 3);

    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1, 12.1},
        {4.7, -2.6, 7.9, 9.57},
        {-5.2, 7.0, 9.1, 4.15}
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1},
        {12.1, 9.5, 4.5} 
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0}
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},        
    };    

    ASSERT_TRUE(m_more_cols.set(more_cols));            
    ASSERT_TRUE(m_more_rows.set(more_rows));            
    ASSERT_TRUE(m_less_cols.set(less_cols));            
    ASSERT_TRUE(m_less_rows.set(less_rows));     

    EXPECT_THROW(matrix.operator==(m_more_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_more_rows), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_rows), std::runtime_error);
}

TEST_F(SquareDefaultMatrix, SumSuccess)
{
    Matrix new_matrix(3, 3);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.1, 9.1}
    };

    std::vector<std::vector<double>> exp_result
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.1, 9.1}
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }                            
}

TEST_F(SquareDefaultMatrix, SumDifferentSize)
{
    Matrix new_matrix(2, 3);
    Matrix result;
    
    EXPECT_THROW(result = matrix.operator+(new_matrix), std::runtime_error); 
}

TEST_F(SquareDefaultMatrix, MulMatricesSuccess)
{
    Matrix new_matrix(3, 3);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.1, 9.1}
    };

    std::vector<std::vector<double>> exp_result
    {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator*(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(result.get(i, j), exp_result[i][j]);        
        }
    }       
}

TEST_F(SquareDefaultMatrix, MulMatricesFailure)
{
    Matrix new_matrix(5, 1);
    EXPECT_THROW(matrix.operator*(new_matrix), std::runtime_error);
}

TEST_F(SquareDefaultMatrix, MulScalar)
{
    Matrix result;
    double scalar = 2.1;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
    };  

    result = matrix.operator*(scalar);
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }               
}

TEST_F(SquareDefaultMatrix, SolveEquationFailure)
{   
    std::vector<double> right_side{0.0};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);                

    right_side[0] = 1.2;        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);       

    right_side.resize(3, 0.0);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);       
}

TEST_F(SquareDefaultMatrix, TransposeMatrix)
{
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}
    }; 

    result = matrix.transpose();
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }  
}

TEST_F(SquareDefaultMatrix, InverseMatrixImpossible)
{          
    EXPECT_THROW(matrix.inverse(), std::runtime_error);           
}

//============================================================================//
// ** SQUARE MATRIX TESTS**
//============================================================================//

TEST_F(SquareMatrix, SetOnPositionSuccess)
{    
    EXPECT_TRUE(matrix.set(0, 0, 12.7));    
    EXPECT_TRUE(matrix.set(2, 2, 0.0));        
    EXPECT_TRUE(matrix.set(1, 2, -7.2));
    EXPECT_TRUE(matrix.set(2, 0, 5.7));
}

TEST_F(SquareMatrix, SetOnPositionFailure)
{            
    EXPECT_FALSE(matrix.set(0, 3, -10.5));
    EXPECT_FALSE(matrix.set(3, 0, 0.5));
    EXPECT_FALSE(matrix.set(5, 10, -7.2));
    EXPECT_FALSE(matrix.set(15, 10, 12.1));
}

TEST_F(SquareMatrix, SetFromVectorSuccess)
{
    std::vector<std::vector<double>> new_matrix
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1}
    };

    EXPECT_TRUE(matrix.set(new_matrix));
}

TEST_F(SquareMatrix, SetFromVectorFailure)
{    
    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1, 12.1},
        {4.7, -2.6, 7.9, 9.57},
        {-5.2, 7.0, 9.1, 4.15}
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1},
        {12.1, 9.5, 4.5} 
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0}
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},        
    };

    EXPECT_FALSE(matrix.set(more_cols));
    EXPECT_FALSE(matrix.set(more_rows));
    EXPECT_FALSE(matrix.set(less_cols));
    EXPECT_FALSE(matrix.set(less_rows));
}   

TEST_F(SquareMatrix, GetSuccess)
{    
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4, -1.5},
        {4.5, 5.7, 2.4},
        {9.2, -7.1, 0.0}
    };

    rows_count = matrix_values.size();    

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = matrix_values[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(matrix.get(i, j), matrix_values[i][j]);        
        }
    }        
}

TEST_F(SquareMatrix, GetFailure)
{
    EXPECT_THROW(matrix.get(0, 3), std::runtime_error);
    EXPECT_THROW(matrix.get(3, 0), std::runtime_error);
    EXPECT_THROW(matrix.get(7, 10), std::runtime_error);
    EXPECT_THROW(matrix.get(17, 5), std::runtime_error);
}

TEST_F(SquareMatrix, EqualIdenticalMatrices)
{
    Matrix new_matrix(3, 3);    
    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4, -1.5},
        {4.5, 5.7, 2.4},
        {9.2, -7.1, 0.0}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_TRUE(matrix.operator==(new_matrix));
}

TEST_F(SquareMatrix, EqualMismatchedMatrices)
{
    Matrix new_matrix(3, 3);    
    std::vector<std::vector<double>> matrix_values
    {
        {0.2, 2.4, 1.5},
        {4.5, 5.17, 2.4},
        {9.02, -7.1, 0.1}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_FALSE(matrix.operator==(new_matrix));
}

TEST_F(SquareMatrix, EqualDifferentSize)
{        
    Matrix m_more_cols(3, 4);
    Matrix m_more_rows(4, 3);
    Matrix m_less_cols(3, 2);
    Matrix m_less_rows(2, 3);

    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1, 12.1},
        {4.7, -2.6, 7.9, 9.57},
        {-5.2, 7.0, 9.1, 4.15}
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1},
        {12.1, 9.5, 4.5} 
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0}
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},        
    };    

    ASSERT_TRUE(m_more_cols.set(more_cols));            
    ASSERT_TRUE(m_more_rows.set(more_rows));            
    ASSERT_TRUE(m_less_cols.set(less_cols));            
    ASSERT_TRUE(m_less_rows.set(less_rows));     

    EXPECT_THROW(matrix.operator==(m_more_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_more_rows), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_rows), std::runtime_error);
}

TEST_F(SquareMatrix, SumSuccess)
{
    Matrix new_matrix(3, 3);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.1, 9.1}
    };

    std::vector<std::vector<double>> exp_result
    {
        {10.2, 3.9, -6.6},
        {9.2, 3.1, 10.3},
        {4.0, 0.0, 9.1}
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }                            
}

TEST_F(SquareMatrix, SumDifferentSize)
{
    Matrix new_matrix(2, 3);    
    
    EXPECT_THROW(matrix.operator+(new_matrix), std::runtime_error); 
}

TEST_F(SquareMatrix, MulMatricesSuccess)
{
    Matrix new_matrix(3, 3);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.1, 9.1}
    };

    std::vector<std::vector<double>> exp_result
    {
        {19.08, -1.59, -46.71},
        {14.31, 8.97, 43.92},
        {-33.37, 32.26, -103.01}
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator*(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(result.get(i, j), exp_result[i][j]);        
        }
    }       
}

TEST_F(SquareMatrix, MulMatricesFailure)
{
    Matrix new_matrix(5, 1);
    EXPECT_THROW(matrix.operator*(new_matrix), std::runtime_error);
}

TEST_F(SquareMatrix, MulScalar)
{
    Matrix result;
    double scalar = 2.1;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {21.42, 5.04, -3.15},
        {9.45, 11.97, 5.04},
        {19.32, -14.91, 0.0}
    };  

    result = matrix.operator*(scalar);
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }               
}

TEST_F(SquareMatrix, SolveEquationSuccessHomogeneous)
{
    std::vector<double> right_side{0.0, 0.0, 0.0};       
    std::vector<double> result;
    std::vector<double> exp_result{0.0, 0.0, 0.0};
    size_t exp_result_size = exp_result.size();

    ASSERT_NO_THROW(result = matrix.solveEquation(right_side));

    for(size_t i = 0; i < exp_result_size; i++)
    {        
        EXPECT_DOUBLE_EQ(exp_result[i], result[i]);        
    }    
}

TEST_F(SquareMatrix, SolveEquationSuccessInhomogeneous)
{
    std::vector<double> right_side{1.2, 5.7, 4.5};       
    std::vector<double> result;
    std::vector<double> exp_result{0.411868076, -0.100114606, 1.840519547};
    size_t exp_result_size = exp_result.size();
    
    ASSERT_NO_THROW(result = matrix.solveEquation(right_side));

    for(size_t i = 0; i < exp_result_size; i++)
    {        
        EXPECT_NEAR(exp_result[i], result[i], 1e-5);              
    }    
}

TEST_F(SquareMatrix, SolveEquationFailure)
{    
    std::vector<double> right_side{1.2};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);        
    
    right_side.resize(2, 7.12);    
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);

    right_side.resize(4, 10.2);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
}

TEST_F(SquareMatrix, TransposeMatrix)
{
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {10.2, 4.5, 9.2},
        {2.4, 5.7, -7.1},
        {-1.5, 2.4, 0.0}
    }; 

    result = matrix.transpose();
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }  
}

TEST_F(SquareMatrix, InverseMatrixSuccess)
{
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {0.048219364, 0.030137103, 0.040494079},
        {0.06248143, 0.039050894, -0.08837387},
        {-0.238804703, 0.267413727, 0.133961543}        
    }; 

    ASSERT_NO_THROW(result = matrix.inverse());
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_NEAR(exp_result[i][j], result.get(i, j), 1e-5);                      
        }
    }      
}

//============================================================================//
// ** MORE ROWS MATRIX TESTS**
//============================================================================//

TEST_F(MoreRowsMatrix, SetOnPositionSuccess)
{    
    EXPECT_TRUE(matrix.set(0, 0, 12.7));    
    EXPECT_TRUE(matrix.set(4, 1, 0.0));        
    EXPECT_TRUE(matrix.set(1, 1, -7.2));
    EXPECT_TRUE(matrix.set(1, 0, 5.7));
}

TEST_F(MoreRowsMatrix, SetOnPositionFailure)
{            
    EXPECT_FALSE(matrix.set(0, 2, -10.5));
    EXPECT_FALSE(matrix.set(5, 0, 0.5));
    EXPECT_FALSE(matrix.set(15, 20, -7.2));
    EXPECT_FALSE(matrix.set(15, 10, 12.1));
}

TEST_F(MoreRowsMatrix, SetFromVectorSuccess)
{
    std::vector<std::vector<double>> new_matrix
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0},
        {-5.1, 7.9},
        {9.1, 5.1}
    };

    EXPECT_TRUE(matrix.set(new_matrix));
}

TEST_F(MoreRowsMatrix, SetFromVectorFailure)
{    
    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1},
        {12.1, 9.57, 4.15},
        {7.0, -5.2, 0.0}
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0},
        {12.1, 9.5},
        {9.1, 5.7},
        {4.5, 0.0}        
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0},
        {4.7},
        {-5.2},
        {12.1},
        {7.0}
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0},
        {12.1, 9.5}                       
    };

    EXPECT_FALSE(matrix.set(more_cols));
    EXPECT_FALSE(matrix.set(more_rows));
    EXPECT_FALSE(matrix.set(less_cols));
    EXPECT_FALSE(matrix.set(less_rows));
}   

TEST_F(MoreRowsMatrix, GetSuccess)
{    
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4},
        {4.5, 5.7},
        {9.2, -7.1},
        {-1.5, 2.4},
        {0.0, 5.1}
    };

    rows_count = matrix_values.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = matrix_values[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(matrix.get(i, j), matrix_values[i][j]);        
        }
    }        
}

TEST_F(MoreRowsMatrix, GetFailure)
{
    EXPECT_THROW(matrix.get(0, 2), std::runtime_error);
    EXPECT_THROW(matrix.get(5, 0), std::runtime_error);
    EXPECT_THROW(matrix.get(7, 10), std::runtime_error);
    EXPECT_THROW(matrix.get(17, 5), std::runtime_error);
}

TEST_F(MoreRowsMatrix, EqualIdenticalMatrices)
{
    Matrix new_matrix(5, 2);    
    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4},
        {4.5, 5.7},
        {9.2, -7.1},
        {-1.5, 2.4},
        {0.0, 5.1}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_TRUE(matrix.operator==(new_matrix));
}

TEST_F(MoreRowsMatrix, EqualMismatchedMatrices)
{
   Matrix new_matrix(5, 2);    
    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4},
        {4.5, 5.7},
        {9.2, -7.1},
        {-1.5, 2.4},
        {0.0, 0.1}
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_FALSE(matrix.operator==(new_matrix));
}

TEST_F(MoreRowsMatrix, EqualDifferentSize)
{        
    Matrix m_more_cols(5, 3);
    Matrix m_more_rows(6, 2);
    Matrix m_less_cols(5, 1);
    Matrix m_less_rows(4, 2);

    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1},
        {4.7, -2.6, 7.9},
        {-5.2, 7.0, 9.1},
        {12.1, 9.57, 4.15},
        {7.0, -5.2, 0.0}
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0},
        {12.1, 9.5},
        {9.1, 5.7},
        {4.5, 0.0}     
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0},
        {4.7},
        {-5.2},
        {12.1},
        {7.0}
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5},
        {4.7, -2.6},
        {-5.2, 7.0},
        {12.1, 9.5}                    
    };    

    ASSERT_TRUE(m_more_cols.set(more_cols));            
    ASSERT_TRUE(m_more_rows.set(more_rows));            
    ASSERT_TRUE(m_less_cols.set(less_cols));            
    ASSERT_TRUE(m_less_rows.set(less_rows));     

    EXPECT_THROW(matrix.operator==(m_more_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_more_rows), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_rows), std::runtime_error);
}

TEST_F(MoreRowsMatrix, SumSuccess)
{
    Matrix new_matrix(5, 2);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5},
        {4.7, -2.67},
        {-5.2, 7.1},
        {-5.1, 7.9},
        {9.1, 0.1}
    };

    std::vector<std::vector<double>> exp_result
    {
        {10.2, 3.9},
        {9.2, 3.03},
        {4.0, 0.0},
        {-6.6, 10.3},
        {9.1, 5.2}
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }                                            
}

TEST_F(MoreRowsMatrix, SumDifferentSize)
{
    Matrix new_matrix(2, 3);        
    EXPECT_THROW(matrix.operator+(new_matrix), std::runtime_error); 
}

TEST_F(MoreRowsMatrix, MulMatricesSuccess)
{
    Matrix new_matrix(2, 2);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5},
        {4.7, -2.6}        
    };

    std::vector<std::vector<double>> exp_result
    {
        {11.28, 9.06},
        {26.79, -8.07},
        {-33.37, 32.26},
        {11.28, -8.49},
        {23.97, -13.26}
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator*(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(result.get(i, j), exp_result[i][j]);        
        }
    }   
}

TEST_F(MoreRowsMatrix, MulMatricesFailure)
{
    Matrix new_matrix(5, 1);
    EXPECT_THROW(matrix.operator*(new_matrix), std::runtime_error);
}

TEST_F(MoreRowsMatrix, MulScalar)
{
    Matrix result;
    double scalar = 2.1;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {21.42, 5.04},
        {9.45, 11.97},
        {19.32, -14.91},
        {-3.15, 5.04},
        {0.0, 10.71}
    };  

    result = matrix.operator*(scalar);
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }        
}

TEST_F(MoreRowsMatrix, SolveEquationFailure)
{    
    std::vector<double> right_side{1.2};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);        
    
    right_side.resize(2, 0.0);    
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);

    right_side.resize(7, 10.2);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
}

TEST_F(MoreRowsMatrix, TransposeMatrix)
{
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {        
        {10.2, 4.5, 9.2, -1.5, 0.0},
        {2.4, 5.7, -7.1, 2.4, 5.1}
    }; 

    result = matrix.transpose();
    rows_count = exp_result.size();
    
    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }  
}

TEST_F(MoreRowsMatrix, InverseMatrixDifferentSize)
{
    EXPECT_THROW(matrix.inverse(), std::runtime_error);
}

//============================================================================//
// ** MORE COLS MATRIX TESTS**
//============================================================================//

TEST_F(MoreColsMatrix, SetOnPositionSuccess)
{    
    EXPECT_TRUE(matrix.set(0, 0, 12.7));    
    EXPECT_TRUE(matrix.set(1, 4, 0.0));        
    EXPECT_TRUE(matrix.set(1, 1, -7.2));
    EXPECT_TRUE(matrix.set(1, 0, 5.7));
}

TEST_F(MoreColsMatrix, SetOnPositionFailure)
{            
    EXPECT_FALSE(matrix.set(0, 5, -10.5));
    EXPECT_FALSE(matrix.set(2, 0, 0.5));
    EXPECT_FALSE(matrix.set(15, 20, -7.2));
    EXPECT_FALSE(matrix.set(15, 10, 12.1));
}

TEST_F(MoreColsMatrix, SetFromVectorSuccess)
{
    std::vector<std::vector<double>> new_matrix
    {
        {0.0, 1.5, 4.7, -2.6, -5.2},
        {7.0, -5.1, 7.9, 9.1, 5.1}        
    };

    EXPECT_TRUE(matrix.set(new_matrix));
}

TEST_F(MoreColsMatrix, SetFromVectorFailure)
{    
    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1, -5.2, 7.0, -5.2},
        {4.7, -2.6, 7.9, 9.57, 4.15, 0.0}        
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5, -5.1, -5.2, 7.0},
        {4.7, -2.6, 7.9, 9.57, 4.15},   
        {0.0, 1.5, 4.7, -2.6, 2.7}
        
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0, 1.5, -5.1, -5.2},
        {4.7, -2.6, 7.9, 9.57}      
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5, -5.1, -5.2, 7.0}        
    };

    EXPECT_FALSE(matrix.set(more_cols));
    EXPECT_FALSE(matrix.set(more_rows));
    EXPECT_FALSE(matrix.set(less_cols));
    EXPECT_FALSE(matrix.set(less_rows));
}   

TEST_F(MoreColsMatrix, GetSuccess)
{    
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4, -1.5, 9.2, -7.1},
        {4.5, 5.7, 2.4, 0.0, 5.1}    
    };

    rows_count = matrix_values.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = matrix_values[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(matrix.get(i, j), matrix_values[i][j]);        
        }
    }        
}

TEST_F(MoreColsMatrix, GetFailure)
{
    EXPECT_THROW(matrix.get(2, 0), std::runtime_error);
    EXPECT_THROW(matrix.get(0, 5), std::runtime_error);
    EXPECT_THROW(matrix.get(7, 10), std::runtime_error);
    EXPECT_THROW(matrix.get(17, 5), std::runtime_error);
}

TEST_F(MoreColsMatrix, EqualIdenticalMatrices)
{
    Matrix new_matrix(2, 5);    
    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4, -1.5, 9.2, -7.1},
        {4.5, 5.7, 2.4, 0.0, 5.1}                
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_TRUE(matrix.operator==(new_matrix));
}

TEST_F(MoreColsMatrix, EqualMismatchedMatrices)
{
    Matrix new_matrix(2, 5);    
    std::vector<std::vector<double>> matrix_values
    {
        {10.2, -2.4, -1.5, 9.2, -7.1},
        {4.25, 5.7, 2.4, 0.1, 7.1}                
    };

    ASSERT_TRUE(new_matrix.set(matrix_values));        
    EXPECT_FALSE(matrix.operator==(new_matrix));
}

TEST_F(MoreColsMatrix, EqualDifferentSize)
{        
    Matrix m_more_cols(2, 6);
    Matrix m_more_rows(3, 5);
    Matrix m_less_cols(2, 4);
    Matrix m_less_rows(1, 5);

    std::vector<std::vector<double>> more_cols
    {
        {0.0, 1.5, -5.1, -5.2, 7.0, -5.2},
        {4.7, -2.6, 7.9, 9.57, 4.15, 0.0}        
    };

    std::vector<std::vector<double>> more_rows
    {
        {0.0, 1.5, -5.1, -5.2, 7.0},
        {4.7, -2.6, 7.9, 9.57, 4.15},   
        {0.0, 1.5, 4.7, -2.6, 2.7}
        
    };

    std::vector<std::vector<double>> less_cols
    {
        {0.0, 1.5, -5.1, -5.2},
        {4.7, -2.6, 7.9, 9.57}      
    };

    std::vector<std::vector<double>> less_rows
    {
        {0.0, 1.5, -5.1, -5.2, 7.0}        
    };

    ASSERT_TRUE(m_more_cols.set(more_cols));            
    ASSERT_TRUE(m_more_rows.set(more_rows));            
    ASSERT_TRUE(m_less_cols.set(less_cols));            
    ASSERT_TRUE(m_less_rows.set(less_rows));     

    EXPECT_THROW(matrix.operator==(m_more_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_more_rows), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_cols), std::runtime_error);
    EXPECT_THROW(matrix.operator==(m_less_rows), std::runtime_error);
}

TEST_F(MoreColsMatrix, SumSuccess)
{
    Matrix new_matrix(2, 5);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {0.0, 1.5, 4.7, -2.67, -5.2},        
        {7.1, -5.0, 7.9, 9.1, 0.1}        
    };

    std::vector<std::vector<double>> exp_result
    {
        {10.2, 3.9, 3.2, 6.53, -12.3},        
        {11.6, 0.7, 10.3, 9.1, 5.2} 
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator+(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));                   
        }
    }                                                     
}

TEST_F(MoreColsMatrix, SumDifferentSize)
{
    Matrix new_matrix(2, 3);        
    EXPECT_THROW(matrix.operator+(new_matrix), std::runtime_error); 
}

TEST_F(MoreColsMatrix, MulMatricesSuccess)
{
    Matrix new_matrix(5, 2);
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> matrix_values
    {
        {10.2, 2.4},
        {4.5, 5.7},
        {9.2, -7.1},
        {-1.5, 2.4},
        {0.0, 5.1}       
    };

    std::vector<std::vector<double>> exp_result
    {
        {87.24, 34.68},
        {93.63, 52.26}        
    };  

    ASSERT_TRUE(new_matrix.set(matrix_values));  
    ASSERT_NO_THROW(result = matrix.operator*(new_matrix));

    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(result.get(i, j), exp_result[i][j]);        
        }
    }   
}

TEST_F(MoreColsMatrix, MulMatricesFailure)
{
    Matrix new_matrix(4, 1);
    EXPECT_THROW(matrix.operator*(new_matrix), std::runtime_error);
}

TEST_F(MoreColsMatrix, MulScalar)
{
    Matrix result;
    double scalar = 2.1;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {21.42, 5.04, -3.15, 19.32, -14.91},
        {9.45, 11.97, 5.04, 0.0, 10.71}             
    };  

    result = matrix.operator*(scalar);
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }   
}

TEST_F(MoreColsMatrix, SolveEquationFailure)
{    
    std::vector<double> right_side{1.2};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);                

    right_side.resize(5, 0.2);    
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);

    right_side.resize(6, 7.1);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
}

TEST_F(MoreColsMatrix, TransposeMatrix)
{
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {        
        {10.2, 4.5},
        {2.4, 5.7},
        {-1.5, 2.4},
        {9.2, 0.0},
        {-7.1, 5.1}        
    }; 

    result = matrix.transpose();
    rows_count = exp_result.size();
    
    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_DOUBLE_EQ(exp_result[i][j], result.get(i, j));        
        }
    }  
}

TEST_F(MoreColsMatrix, InverseMatrixDifferentSize)
{
   EXPECT_THROW(matrix.inverse(), std::runtime_error); 
}

//============================================================================//
// ** SPECIAL CASES TESTS**
//============================================================================//

// ** NEGATIVE DETERMINANT MATRIX **
TEST_F(NegativeDeterminantMatrix, SolveEquationSuccessHomogeneous)
{
    std::vector<double> right_side{0.0, 0.0, 0.0};       
    std::vector<double> result;
    std::vector<double> exp_result{0.0, 0.0, 0.0};
    size_t exp_result_size = exp_result.size();    
    
    ASSERT_NO_THROW(result = matrix.solveEquation(right_side));

    for(size_t i = 0; i < exp_result_size; i++)
    {        
        EXPECT_DOUBLE_EQ(exp_result[i], result[i]);        
    }     
}

TEST_F(NegativeDeterminantMatrix, SolveEquationSuccessInhomogeneous)
{
    std::vector<double> right_side{1.2, 5.7, 4.5};       
    std::vector<double> result;
    std::vector<double> exp_result{-1.61574583, -2.7274453, 5.82315916};
    size_t exp_result_size = exp_result.size(); 
    
    ASSERT_NO_THROW(result = matrix.solveEquation(right_side));

    for(size_t i = 0; i < exp_result_size; i++)
    {        
        EXPECT_NEAR(exp_result[i], result[i], 1e-5);              
    }    
}

TEST_F(NegativeDeterminantMatrix, SolveEquationFailure)
{    
    std::vector<double> right_side{1.2};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);        
    
    right_side.resize(2, 7.12);    
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);

    right_side.resize(4, 10.2);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
}

// ** TWO DIMENSIONAL SQUARE MATRIX ** 
TEST_F(TwoDimensionalSquareMatrix, InverseMatrixSuccess)
{
    Matrix result;
    size_t rows_count;
    size_t cols_count;

    std::vector<std::vector<double>> exp_result
    {
        {0.082144401, 0.036028246},
        {-0.064850843, 0.146995244}        
    }; 

    ASSERT_NO_THROW(result = matrix.inverse());
    
    rows_count = exp_result.size();

    for(size_t i = 0; i < rows_count; i++)
    {
        cols_count = exp_result[i].size();

        for(size_t j = 0; j < cols_count; j++)
        {
            EXPECT_NEAR(exp_result[i][j], result.get(i, j), 1e-5);                      
        }
    }    
}

TEST_F(TwoDimensionalSquareMatrix, InverseMatrixImpossible)
{
    std::vector<std::vector<double>> new_matrix
    {
        {0.0, 0.0},
        {0.0, 0.0}        
    };

    ASSERT_TRUE(matrix.set(new_matrix));    
    EXPECT_THROW(matrix.inverse(), std::runtime_error);
}

// ** FOUR DIMENSIONAL SQUARE MATRIX ** 
TEST_F(FourDimensionalSquareMatrix, InverseMatrixDifferentSize)
{
   EXPECT_THROW(matrix.inverse(), std::runtime_error); 
}

TEST_F(FourDimensionalSquareMatrix, SolveEquationSuccessHomogeneous)
{
    std::vector<double> right_side{0.0, 0.0, 0.0, 0.0};       
    std::vector<double> result;
    std::vector<double> exp_result{0.0, 0.0, 0.0, 0.0};
    size_t exp_result_size = exp_result.size();

    ASSERT_NO_THROW(result = matrix.solveEquation(right_side));

    for(size_t i = 0; i < exp_result_size; i++)
    {        
        EXPECT_DOUBLE_EQ(exp_result[i], result[i]);        
    }    
}

TEST_F(FourDimensionalSquareMatrix, SolveEquationSuccessInhomogeneous)
{
    std::vector<double> right_side{1.2, 5.7, 4.5, 0.1};       
    std::vector<double> result;
    std::vector<double> exp_result{2.46821, 3.22065, -2.01645, 2.74062};
    size_t exp_result_size = exp_result.size();
    
    ASSERT_NO_THROW(result = matrix.solveEquation(right_side));

    for(size_t i = 0; i < exp_result_size; i++)
    {        
        EXPECT_NEAR(exp_result[i], result[i], 1e-5);              
    }    
}

TEST_F(FourDimensionalSquareMatrix, SolveEquationFailure)
{    
    std::vector<double> right_side{1.2};        
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);                

    right_side.resize(5, 10.2);
    EXPECT_THROW(matrix.solveEquation(right_side), std::runtime_error);
}

/*** Konec souboru white_box_tests.cpp ***/
