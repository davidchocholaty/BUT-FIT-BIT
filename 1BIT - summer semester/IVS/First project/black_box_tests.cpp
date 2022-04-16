//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     DAVID CHOCHOLATY <xchoch09@stud.fit.vutbr.cz>
// $Date:       $2021-02-27
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author DAVID CHOCHOLATY
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"


#define ELEMENTSCOUNT 10

class EmptyTree : public ::testing::Test
{
    protected:
        BinaryTree binary_tree;
};

class NonEmptyTree : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            for (int i = 0; i < ELEMENTSCOUNT; i++)
            {
                binary_tree.InsertNode(i);
            }            
        };

        BinaryTree binary_tree;
};

class TreeAxioms : public ::testing::Test
{    
    protected:    
        virtual void SetUp()
        {
            for (int i = 0; i < ELEMENTSCOUNT; i++)
            {
                binary_tree.InsertNode(i);
            }            
        };

        BinaryTree binary_tree;
};


TEST_F(EmptyTree, InsertNode_NewValue)
{
    std::pair <bool, BinaryTree::Node_t *> pair;    

    pair = binary_tree.InsertNode(2);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(3);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(-4);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(-5);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);
}

TEST_F(EmptyTree, InsertNode_SameValue)
{
    std::pair <bool, BinaryTree::Node_t *> pair;

    pair = binary_tree.InsertNode(2);
    ASSERT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);
    
    pair = binary_tree.InsertNode(2);
    EXPECT_EQ(pair.first, false);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(-4);
    ASSERT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(-4);
    EXPECT_EQ(pair.first, false);
    EXPECT_TRUE(pair.second != NULL);
}

TEST_F(EmptyTree, DeleteNode)
{
    std::pair <bool, BinaryTree::Node_t *> pair;

    // Test odstraneni prvku z prazdneho stromu.
    EXPECT_FALSE(binary_tree.DeleteNode(2));
    EXPECT_FALSE(binary_tree.DeleteNode(-3));
    EXPECT_FALSE(binary_tree.DeleteNode(4));
    EXPECT_FALSE(binary_tree.DeleteNode(-5));

    /*
     * Vlozeni prvku pro ucely testovani
     * jejich nasledneho odstraneni.
     */ 
    pair = binary_tree.InsertNode(2);
    ASSERT_EQ(pair.first, true);
    ASSERT_TRUE(pair.second != NULL);    

    pair = binary_tree.InsertNode(4);
    ASSERT_EQ(pair.first, true);
    ASSERT_TRUE(pair.second != NULL);

    /*
     * Odstraneni vlozenych prvku 2 a 4.
     */ 
    EXPECT_FALSE(binary_tree.DeleteNode(1)); // Test odstraneni nevlozeneho prvku 1.
    EXPECT_TRUE(binary_tree.DeleteNode(2));    
    EXPECT_FALSE(binary_tree.DeleteNode(-3)); // Test odstraneni nevlozeneho prvku -3.
    EXPECT_TRUE(binary_tree.DeleteNode(4));
    EXPECT_FALSE(binary_tree.DeleteNode(5)); // Test odstraneni nevlozeneho prvku 5.
}

TEST_F(EmptyTree, FindNode)
{
    std::pair <bool, BinaryTree::Node_t *> pair;
    
    EXPECT_EQ(binary_tree.FindNode(2), nullptr);    
    EXPECT_EQ(binary_tree.FindNode(3), nullptr);
    EXPECT_EQ(binary_tree.FindNode(4), nullptr);
    EXPECT_EQ(binary_tree.FindNode(5), nullptr);

    /*
     * Vlozeni prvku pro ucely testovani
     * jejich nasledneho vyhledavani.
     */ 
    pair = binary_tree.InsertNode(2);
    ASSERT_EQ(pair.first, true);
    ASSERT_TRUE(pair.second != NULL);    

    pair = binary_tree.InsertNode(4);
    ASSERT_EQ(pair.first, true);
    ASSERT_TRUE(pair.second != NULL);

    /*
     * Vyhledani vlozenych prvku 2 a 4.
     */     
    EXPECT_EQ(binary_tree.FindNode(1), nullptr); // Test vyhledani nevlozeneho prvku 1.
    EXPECT_NE(binary_tree.FindNode(2), nullptr);
    EXPECT_EQ(binary_tree.FindNode(-3), nullptr); // Test vyhledani nevlozeneho prvku -3.
    EXPECT_NE(binary_tree.FindNode(4), nullptr);
    EXPECT_EQ(binary_tree.FindNode(5), nullptr); // Test vyhledani nevlozeneho prvku 5.         
}

TEST_F(NonEmptyTree, InsertNode_NewValue)
{
    std::pair <bool, BinaryTree::Node_t *> pair;    

    pair = binary_tree.InsertNode(10);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(11);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(22);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(24);
    EXPECT_EQ(pair.first, true);
    EXPECT_TRUE(pair.second != NULL);
}

TEST_F(NonEmptyTree, InsertNode_SameValue)
{
    std::pair <bool, BinaryTree::Node_t *> pair;

    pair = binary_tree.InsertNode(0);
    EXPECT_EQ(pair.first, false);
    EXPECT_TRUE(pair.second != NULL);
    
    pair = binary_tree.InsertNode(2);
    EXPECT_EQ(pair.first, false);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(5);
    EXPECT_EQ(pair.first, false);
    EXPECT_TRUE(pair.second != NULL);

    pair = binary_tree.InsertNode(9);
    EXPECT_EQ(pair.first, false);
    EXPECT_TRUE(pair.second != NULL);
}

TEST_F(NonEmptyTree, DeleteNode_SingleElement)
{         
    /*
     * Odstraneni vlozenych prvku 0, 4, 5, 7, 9.
     */ 
    EXPECT_FALSE(binary_tree.DeleteNode(-1)); // Test odstraneni nevlozeneho prvku -1.
    EXPECT_TRUE(binary_tree.DeleteNode(0));    
    EXPECT_FALSE(binary_tree.DeleteNode(10)); // Test odstraneni nevlozeneho prvku 10.
    EXPECT_TRUE(binary_tree.DeleteNode(4));
    EXPECT_FALSE(binary_tree.DeleteNode(-5)); // Test odstraneni nevlozeneho prvku -5.
    EXPECT_TRUE(binary_tree.DeleteNode(5));
    EXPECT_FALSE(binary_tree.DeleteNode(15)); // Test odstraneni nevlozeneho prvku 15.
    EXPECT_TRUE(binary_tree.DeleteNode(7));
    EXPECT_FALSE(binary_tree.DeleteNode(-22)); // Test odstraneni nevlozeneho prvku -22.
    EXPECT_TRUE(binary_tree.DeleteNode(9));
    EXPECT_FALSE(binary_tree.DeleteNode(57)); // Test odstraneni nevlozeneho prvku 57.
}

TEST_F(NonEmptyTree, DeleteNode_RemoveAllFromSmallest)
{
    for (int i = 0; i < ELEMENTSCOUNT; i++)
    {
        EXPECT_TRUE(binary_tree.DeleteNode(i));    
    }
    
    EXPECT_EQ(binary_tree.GetRoot(), nullptr);
}

TEST_F(NonEmptyTree, DeleteNode_RemoveAllFromLargest)
{
    for (int i = ELEMENTSCOUNT - 1; i >= 0; i--)
    {
        EXPECT_TRUE(binary_tree.DeleteNode(i));    
    }

    EXPECT_EQ(binary_tree.GetRoot(), nullptr);
}

TEST_F(NonEmptyTree, FindNode)
{
    int count;
    
    /*
     * Test vyhledani vlozenych prvku.
     */ 
    for (int i = 0; i < ELEMENTSCOUNT; i++)
    {
        EXPECT_NE(binary_tree.FindNode(i), nullptr);
    }

    /*
     * Test vyhledani nevlozenych prvku mensich nez vlozenych.
     */ 
    count = (-1) * ELEMENTSCOUNT;
    for (int i = -1; i > count; i--)
    {
        EXPECT_EQ(binary_tree.FindNode(i), nullptr);
    }        

    /*
     * Test vyhledani nevlozenych prvku vetsich nez vlozenych.
     */
    count = 2 * ELEMENTSCOUNT;
    for (int i = ELEMENTSCOUNT; i < count; i++)
    {
        EXPECT_EQ(binary_tree.FindNode(i), nullptr);
    }                    
}

TEST_F(TreeAxioms, Axiom1)
{
    std::vector<BinaryTree::Node_t*> leaf_node;
    size_t length;

    binary_tree.GetLeafNodes(leaf_node);
    length = leaf_node.size();

    for (size_t i = 0; i < length; i++)
    {
        EXPECT_EQ(BinaryTree::BLACK, leaf_node[i]->color);
    }    
}

TEST_F(TreeAxioms, Axiom2)
{
    std::vector<BinaryTree::Node_t*> non_leaf_node;
    size_t length;

    binary_tree.GetNonLeafNodes(non_leaf_node);
    length = non_leaf_node.size();

    for (size_t i = 0; i < length; i++)
    {
        if(non_leaf_node[i]->color == BinaryTree::RED)
        {            
            EXPECT_EQ(non_leaf_node[i]->pLeft->color, BinaryTree::BLACK);            
            EXPECT_EQ(non_leaf_node[i]->pRight->color, BinaryTree::BLACK);            
        }
    }    
}

TEST_F(TreeAxioms, Axiom3)
{
    std::vector<BinaryTree::Node_t*> leaf_node;
    Node_t *node;
    size_t exp_count = 0;
    size_t count = 0;   
    size_t length; 

    binary_tree.GetLeafNodes(leaf_node);    
    length = leaf_node.size();

    if(length != 0)
    {
        node = leaf_node[0];

        /*
         * Ziskani poctu cernych uzlu od prvniho listoveho uzlu ke koreni.
         */ 
        while (node != binary_tree.GetRoot())
        {
            node = node->pParent;

            if(node->color == BinaryTree::BLACK)
            {            
                exp_count++;
            }            
        }

        /*
         * Overeni stejneho poctu cernych uzlu od listoveho uzlu ke koreni
         * u dalsich listovych uzlu (krome prvniho listoveho uzlu).
         */
        for (size_t i = 1; i < length; i++)
        {
            node = leaf_node[i];

            while (node != binary_tree.GetRoot())
            {
                node = node->pParent;

                if(node->color == BinaryTree::BLACK)
                {            
                    count++;
                }
            }        

            EXPECT_EQ(exp_count, count);
            count = 0;
        }    
    }    
}

/*** Konec souboru black_box_tests.cpp ***/
