---
title: "VSS Documentation"
date: 2019-08-04T13:21:34+02:00
weight: 3
---

The VSS documentation is realized with GitHub Pages. It is generated from
the markdown files in the ```/documentation``` directory of this repositories.
The static webpage is generated into the ```/docs``` directory so that it can
be deployed straight from the repository.


### Dependencies

The static page is generated with:

- [HUGO](https://gohugo.io/)
- [Learn Theme](https://themes.gohugo.io/hugo-theme-learn/)

Please follow the [documentation](https://gohugo.io/documentation/) for
installation and further questions around the framework.


### Run the documentation server locally

Once hugo is installed please follow the following steps:

1. **Check that HUGO is working:**
```
hugo version
```
The following outcome is expected:
```
Hugo Static Site Generator v0.xx.xx ...
```
1. **Clone the submodule containing the theme** </br>
Run the following git commands to init and fetch the submodules:
```
git submodule init
git submodule update
```
Reference: [Git Documentation](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

1. **Test locally on your server:**
```
hugo server -D
```
Optional ```-D:``` include draft pages as well. Afterwards, you can access the
page under http://localhost:1313/vehicle_signal_specification.

### Contribute

Right now there is no pipeline in place to build the documentation. If you want
to contribute, do the following:

1. **Change documentation in ```/documentation```**

1. **Delete the current ```/docs```folder**
```
rm -r <vss_repo>/docs
```

1. **Build the documentation**
```
cd <vss_repo>/tools/documentation
hugo
ls ../../docs
```
Now the ```<vss_repo>/docs``` folder should contain the documentation.

1. **Create Pull Request for review**
