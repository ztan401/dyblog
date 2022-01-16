<template>
  <el-container>
    <!-- 头部 -->
    <el-header>
      <div>
        <img src="../assets/logo.png" alt />
        <span>Common Analytics Library</span>
      </div>
      <el-button type="info" @click="logout" size="small">退出</el-button>
    </el-header>
    <!-- 主体 -->
    <el-container>
      <!-- 侧边栏 -->
      <el-aside :width="isCollapse ? '64px' : '240px'">
        <div class="toggle-button" @click="togleCollapse">|||</div>
        <el-menu
          unique-opened
          :collapse="isCollapse"
          :collapse-transition="false"
          router
          background-color="#333744"
          text-color="#fff"
          active-text-color="#409FFF"
        >
          <!-- :unique-opened="true"->只允许展开一个菜单 -->
          <!-- :collapse-transition="false" -> 关闭动画 -->
          <!-- router -> 导航开启路由模式 -->
          <!-- 一级菜单  -->
          <el-submenu
            :index="item.id + ''"
            v-for="item in menuList"
            :key="item.id"
          >
            <!-- 一级菜单的模板区域 -->
            <template slot="title">
              <i :class="iconObj[item.id]"></i>
              <span>{{ item.authName }}</span>
            </template>
            <!-- 二级菜单 -->
            <el-menu-item
              :index="'/' + subItem.path"
              v-for="subItem in item.children"
              :key="subItem.id"
              @click="saveNavState('/' + subItem.path)"
            >
              <!-- 导航开启路由模式：
              将index值作为导航路由-->
              <!-- 二级菜单的模板区域 -->
              <template slot="title">
                <i class="el-icon-menu"></i>
                <span>{{ subItem.authName }}</span>
              </template>
            </el-menu-item>
          </el-submenu>
        </el-menu>
      </el-aside>
      <!-- 内容主体 -->
      <el-main>
        <router-view></router-view>
      </el-main>
    </el-container>
  </el-container>
</template>

<script>
// import left_menu from "../config/left_menu.json";
const left_menu = require('../config/left_menu.json')

export default {
  data () {
    return {
      // 左侧菜单数据
      menuList: [],
      iconObj: {
        125: 'iconfont icon-user',
        101: 'iconfont icon-shangpin',
        102: 'iconfont icon-danju',
        103: 'iconfont icon-tijikongjian',
        104: 'iconfont icon-morentouxiang',
        105: 'iconfont icon-shangpin',
        106: 'iconfont icon-danju',
        107: 'iconfont icon-tijikongjian',
        145: 'iconfont icon-baobiao'
      },
      // 默认不折叠
      isCollapse: true
      // 被激活导航地址
      // activePath: "",
    }
  },
  created () {
    this.getMenuList()
    // this.activePath = window.sessionStorage.getItem("activePath");
  },
  methods: {
    logout () {
      // 清空token
      // window.sessionStorage.clear();
      this.$router.push('/login')
    },
    // 获取请求菜单
    async getMenuList () {
      this.menuList = left_menu.data
    },
    // 菜单的折叠与展开
    togleCollapse () {
      this.isCollapse = !this.isCollapse
    }
    // // 保存连接的激活地址
    // saveNavState(activePath) {
    //   window.sessionStorage.setItem("activePath", activePath);
    // },
  }
}
</script>

<style lang="less" scoped>
.el-container {
  min-height: 100%;
}
.el-header {
  background-color: #373f41;
  display: flex;
  justify-content: space-between;
  padding-left: 0;
  align-items: center;
  color: #fff;
  font-size: 20px;
  > div {
    display: flex;
    align-items: center;
    img {
      height: 40px;
    }
    span {
      margin-left: 15px;
    }
  }
}
.el-aside {
  background-color: #333744;

  .el-menu {
    border: none;
  }
}
.el-main {
  background-color: #eaedf1;
}
.iconfont {
  margin-right: 10px;
}
.toggle-button {
  background-color: #4a5064;
  font-size: 10px;
  line-height: 24px;
  color: #fff;
  text-align: center;
  letter-spacing: 0.2em;
  // 鼠标放上去变成小手
  cursor: pointer;
}
</style>
